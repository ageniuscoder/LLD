# ConcurrentHashMap & CopyOnWriteArrayList in Go

> Java concepts mapped to their Go equivalents — with code, tradeoffs, and guidance on when to use each.

---

## Table of Contents

- [Background](#background)
- [ConcurrentHashMap → Go Equivalents](#concurrenthashmap--go-equivalents)
  - [Option 1: sync.RWMutex + map](#option-1-syncrwmutex--map)
  - [Option 2: sync.Map](#option-2-syncmap)
  - [Option 3: Sharded Map](#option-3-sharded-map)
- [CopyOnWriteArrayList → Go Equivalent](#copyonwritearraylist--go-equivalent)
- [Comparison Table](#comparison-table)
- [Applied to TaskManager Project](#applied-to-taskmanager-project)

---

## Background

These are two foundational Java concurrency utilities:

| Java Type              | Purpose                                                       |
| ---------------------- | ------------------------------------------------------------- |
| `ConcurrentHashMap`    | Thread-safe map — multiple readers, exclusive writers         |
| `CopyOnWriteArrayList` | Thread-safe list — writes copy the array, reads are lock-free |

Go has no direct built-in equivalents by name, but all the same guarantees are achievable with `sync`, `sync/atomic`, and standard maps/slices.

---

## ConcurrentHashMap → Go Equivalents

Go provides **three approaches**, each with different complexity/performance tradeoffs.

---

### Option 1: `sync.RWMutex` + `map`

The most common and idiomatic Go approach. Gives you full type safety and control.

Multiple goroutines can call `RLock()` for reads simultaneously. Writers take an exclusive `Lock()` that blocks all readers and other writers.

```go
type SafeMap struct {
    mu   sync.RWMutex
    data map[string]*Task
}

func (m *SafeMap) Get(key string) (*Task, bool) {
    m.mu.RLock()
    defer m.mu.RUnlock()
    v, ok := m.data[key]
    return v, ok
}

func (m *SafeMap) Set(key string, val *Task) {
    m.mu.Lock()
    defer m.mu.Unlock()
    m.data[key] = val
}

func (m *SafeMap) Delete(key string) {
    m.mu.Lock()
    defer m.mu.Unlock()
    delete(m.data, key)
}
```

**When to use:** General purpose. Best choice for typed domain models where you need full control over locking granularity.

> **Rule:** Every map access — read or write — must be inside a lock. The lock must cover the entire check-then-act sequence, not just the write half.

---

### Option 2: `sync.Map`

Go's built-in concurrent map, optimised for two specific access patterns:

- A key is **written once and read many times**
- Goroutines work on **disjoint sets of keys** (no two goroutines share the same key)

It uses internal sharding and lock-free reads via atomic pointers internally.

```go
var m sync.Map

// Store a value
m.Store("task1", &Task{tid: "task1"})

// Load a value
val, ok := m.Load("task1")
if ok {
    task := val.(*Task) // type assertion required — sync.Map stores interface{}
    fmt.Println(task.tid)
}

// Delete a value
m.Delete("task1")

// Iterate — safe, operates on a snapshot internally
m.Range(func(key, value any) bool {
    task := value.(*Task)
    fmt.Println(key, task.tid)
    return true // return false to stop iteration early
})
```

**Tradeoff:** `sync.Map` uses `any` (empty interface), so you lose compile-time type safety and must type-assert on every read. For typed domain models, `RWMutex + map` is cleaner.

**When to use:** Configuration maps, caches, or registries that are written rarely and read constantly.

---

### Option 3: Sharded Map

For extreme write throughput, split the map into N independent shards, each with its own mutex. Writers on different shards never block each other, reducing contention by a factor of N.

> This is exactly how Java's `ConcurrentHashMap` works internally — it segments the underlying array into buckets, each with its own lock.

```go
const shardCount = 16

type Shard struct {
    mu   sync.RWMutex
    data map[string]*Task
}

type ShardedMap struct {
    shards [shardCount]*Shard
}

func NewShardedMap() *ShardedMap {
    sm := &ShardedMap{}
    for i := range sm.shards {
        sm.shards[i] = &Shard{data: make(map[string]*Task)}
    }
    return sm
}

// getShard hashes the key to a shard index
func (sm *ShardedMap) getShard(key string) *Shard {
    var h uint32
    for _, c := range key {
        h = h*31 + uint32(c)
    }
    return sm.shards[h%shardCount]
}

func (sm *ShardedMap) Get(key string) (*Task, bool) {
    shard := sm.getShard(key)
    shard.mu.RLock()
    defer shard.mu.RUnlock()
    v, ok := shard.data[key]
    return v, ok
}

func (sm *ShardedMap) Set(key string, val *Task) {
    shard := sm.getShard(key)
    shard.mu.Lock()
    defer shard.mu.Unlock()
    shard.data[key] = val
}

func (sm *ShardedMap) Delete(key string) {
    shard := sm.getShard(key)
    shard.mu.Lock()
    defer shard.mu.Unlock()
    delete(shard.data, key)
}
```

**When to use:** Very high write concurrency — e.g. a hot cache or session store with thousands of concurrent writers. Overkill for most applications.

---

## CopyOnWriteArrayList → Go Equivalent

The core idea: **on every write, copy the entire slice, mutate the copy, then atomically swap the pointer**. Readers always see a consistent snapshot and need zero locking.

```
Write path:  Lock → copy old slice → mutate copy → atomic pointer swap → Unlock
Read path:   atomic pointer load → range over snapshot (no lock at all)
```

```go
import (
    "sync"
    "sync/atomic"
)

type COWList struct {
    ptr atomic.Pointer[[]string] // atomic pointer to the current slice
    mu  sync.Mutex               // serialises writers — only one writer at a time
}

func NewCOWList() *COWList {
    l := &COWList{}
    empty := make([]string, 0)
    l.ptr.Store(&empty)
    return l
}

// ReadAll — completely lock-free, readers never block writers or each other
func (l *COWList) ReadAll() []string {
    return *l.ptr.Load() // returns current snapshot — safe to range over
}

// Append — copies the slice, appends the new element, atomically swaps pointer
func (l *COWList) Append(val string) {
    l.mu.Lock()
    defer l.mu.Unlock()

    old := *l.ptr.Load()
    newSlice := make([]string, len(old)+1)
    copy(newSlice, old)
    newSlice[len(old)] = val
    l.ptr.Store(&newSlice) // atomic swap — readers instantly see new slice
}

// Remove — copies the slice without the matching element, atomically swaps pointer
func (l *COWList) Remove(val string) {
    l.mu.Lock()
    defer l.mu.Unlock()

    old := *l.ptr.Load()
    newSlice := make([]string, 0, len(old))
    for _, v := range old {
        if v != val {
            newSlice = append(newSlice, v)
        }
    }
    l.ptr.Store(&newSlice)
}
```

**Usage:**

```go
list := NewCOWList()
list.Append("task1")
list.Append("task2")

// Multiple goroutines read with zero locking
var wg sync.WaitGroup
for i := 0; i < 10; i++ {
    wg.Add(1)
    go func() {
        defer wg.Done()
        items := list.ReadAll() // gets a snapshot — no lock needed
        for _, item := range items {
            fmt.Println(item)
        }
    }()
}
wg.Wait()
```

**When to use:**

- Read operations vastly outnumber writes (e.g. 100:1 or more)
- You need lock-free reads even under concurrent writes
- The list is small enough that copying on every write is cheap

**When NOT to use:**

- Writes are frequent — every write allocates a new slice (GC pressure)
- The list is very large — copying is O(n) on every write

---

## Comparison Table

| Java                            | Go Equivalent                   | Type Safe         | Read Perf                   | Write Perf            | Best For                            |
| ------------------------------- | ------------------------------- | ----------------- | --------------------------- | --------------------- | ----------------------------------- |
| `ConcurrentHashMap`             | `sync.RWMutex` + `map`          | Yes               | High (shared read lock)     | Medium                | General purpose, typed models       |
| `ConcurrentHashMap`             | `sync.Map`                      | No (needs assert) | Very high (lock-free reads) | Medium                | Write-once-read-many, disjoint keys |
| `ConcurrentHashMap` (high perf) | Sharded map                     | Yes               | High                        | Very high             | Extreme write concurrency           |
| `CopyOnWriteArrayList`          | `atomic.Pointer` + `sync.Mutex` | Yes               | Highest (no lock at all)    | Low (copies on write) | Read-heavy lists, rare writes       |
| `CopyOnWriteArrayList` (simple) | `sync.RWMutex` + slice          | Yes               | High (shared read lock)     | Low-Medium            | Simpler read-heavy lists            |

---

## Applied to TaskManager Project

The `TaskManager` struct uses `map[string][]*TaskHistory` for `assignedHistory` and `completedHistory`, protected by a single `sync.RWMutex`.

```go
type TaskManager struct {
    tasks            map[string]*Task
    assignedHistory  map[string][]*TaskHistory  // ← map of slices
    completedHistory map[string][]*TaskHistory
    rwm              sync.RWMutex
}
```

### Current approach: `RWMutex` + map ✓

Correct for the current scale. Every read is under `RLock()`, every write is under `Lock()`, and `notify()` is always called after the lock is released to avoid holding locks during I/O.

### When to consider upgrading

| Scenario                                      | Recommended Change                                          |
| --------------------------------------------- | ----------------------------------------------------------- |
| Tasks map has thousands of concurrent writers | Move `tasks` to a `ShardedMap`                              |
| History is polled by a dashboard constantly   | Make each user's queue a `COWList` — reads become zero-lock |
| Task lookup is write-once, read-many          | Move `tasks` to `sync.Map`                                  |

For the current project scale, `RWMutex + map` is the right choice. The COW and sharded patterns pay off only when profiling shows lock contention as a bottleneck.

---

> **Key principle:** Always match the concurrency primitive to your actual read/write ratio. Reaching for `sync.Map` or COW without profiling first is premature optimisation.
