# Edge Cases and Invariants

## Core invariants

### Invariant 1

A seat cannot have two simultaneous owners.

```text
seat -> at most one active hold
```

### Invariant 2

A booked seat cannot be acquired again.

```text
BOOKED -> TryHold = false
```

### Invariant 3

Only the hold owner can confirm.

```text
HELD by Alice
Bob -> Confirm = false
Alice -> Confirm = possible
```

### Invariant 4

Expired holds cannot be confirmed.

```text
HELD
 + timeout exceeded
 = invalid
```

### Invariant 5

Multi-seat hold is atomic.

Either:

```text
all seats HELD
```

or:

```text
none of them remain HELD by that request
```

---

# Edge cases

## Invalid seat ID

```text
User -> s-999
```

Return failure rather than dereferencing nil.

---

## Empty seat list

```text
Book(user, [])
```

Usually reject.

---

## Duplicate seat IDs

```text
[s-11, s-11]
```

Reject or normalize before acquisition.

Rejecting is simpler for an interview.

---

## Already booked seat

```text
BOOKED -> Hold()
```

must fail.

---

## Already held seat

```text
HELD by Alice
Bob -> Hold()
```

must fail.

---

## Same user tries to hold again

Choose and state an assumption.

Simple interview choice:

```text
A second hold request is treated as a new request and fails if
the seat is already held.
```

Alternatively, support idempotency with a hold ID.

---

## Payment failure

```text
HELD -> payment failure -> AVAILABLE
```

---

## Payment success after expiry

Must fail:

```text
HELD -> expired -> AVAILABLE
payment confirmation -> false
```

---

## Duplicate confirmation

```text
BOOKED -> Confirm()
```

should fail or return an idempotent success depending on the API contract.

For a simple interview implementation, return false.

---

# Lock ordering

Always acquire multiple seat locks in a deterministic order.

For example:

```text
s-11 < s-12 < s-13
```

Never depend on the order supplied by the client.

---

# Lock scope

Do not hold a lock while performing a slow external operation such as:

```text
payment gateway call
network request
database request
```

Instead:

```text
hold seats
   ↓
release seat locks while payment happens
   ↓
confirm atomically
```

The system must use the HELD state and expiry validation to protect the seats while payment is in progress.

---

# Important production note

A process-local `sync.Mutex` only protects one application instance.

If the application runs as:

```text
Server A
Server B
Server C
```

then:

```go
sync.Mutex
```

on Server A does not protect Server B.

For a distributed production system, the final source of truth must provide cross-instance atomicity, commonly through a database transaction/row locking strategy or another carefully designed distributed coordination mechanism.

For the interview's single-process concurrency requirement, per-seat mutexes are sufficient.
