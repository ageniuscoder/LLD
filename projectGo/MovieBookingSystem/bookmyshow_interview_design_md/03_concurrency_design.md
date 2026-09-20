# Concurrency Design

## Primary requirement

For a given show:

> Two users must never successfully acquire the same seat at the same time.

---

## Option A — Per-seat lock

The preferred interview design for this problem is a lock per seat.

```text
ShowSeat
   |
   +-- s-11 -> SeatState + Lock
   +-- s-12 -> SeatState + Lock
   +-- s-13 -> SeatState + Lock
```

This allows:

```text
User A -> s-11
User B -> s-12
```

to proceed concurrently.

It avoids unnecessarily locking the entire show.

---

## SeatState transition

Conceptually:

```text
AVAILABLE
   |
   | acquire lock
   | verify AVAILABLE
   | set owner
   v
HELD
```

The lock protects the transition.

---

## Important invariant

For every seat:

```text
At most one owner may hold the seat lock.
```

Therefore:

```text
User A -> TryLock(s-11) -> success
User B -> TryLock(s-11) -> failure
```

---

# Multi-seat booking

Suppose:

```text
User A -> [s-11, s-12, s-13]
```

We need all-or-nothing acquisition.

Algorithm:

```text
1. Sort seat IDs.
2. Acquire each seat in sorted order.
3. If any acquisition fails:
      release every seat acquired by this request.
4. If all succeed:
      mark all seats HELD.
```

Pseudo-code:

```go
func Hold(user string, seatIDs []string) bool {
    sort.Strings(seatIDs)

    acquired := make([]string, 0)

    for _, id := range seatIDs {
        if !tryHold(id, user) {
            release(acquired)
            return false
        }

        acquired = append(acquired, id)
    }

    return true
}
```

---

# Why sorting matters

Without a fixed acquisition order:

```text
User A:
lock A
wait B

User B:
lock B
wait A
```

Both can wait forever.

This is a classic deadlock.

With sorting:

```text
User A: A -> B
User B: A -> B
```

Only one can acquire A first.

The second fails immediately instead of forming a cycle.

---

# Seat-level locking vs show-level locking

### Show-level mutex

```text
Show
  |
  +-- mutex
```

Simple but serializes:

```text
User A -> s-11
User B -> s-12
User C -> s-13
```

even though the operations are independent.

### Seat-level mutex

```text
s-11 -> mutex
s-12 -> mutex
s-13 -> mutex
```

Allows independent seats to proceed concurrently.

For this interview problem, seat-level locking demonstrates better concurrency reasoning.

---

# Do not confuse data-race safety with business correctness

This is not enough:

```go
mutex.Lock()
readState()
mutex.Unlock()
```

followed later by:

```go
mutex.Lock()
writeState()
mutex.Unlock()
```

If another goroutine can change the business state between those operations, the overall transaction may still be incorrect.

The important business transition is:

```text
HELD by Alice
       |
       | validate owner + validate expiry + confirm
       v
BOOKED
```

That transition must be synchronized as one logical operation.
