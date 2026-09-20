# Interview Implementation Order

If the interviewer gives approximately 45–60 minutes, do not try to implement the entire product.

Implement in this order.

---

## 1. Enum/state definitions

```text
SeatStatus:
    AVAILABLE
    HELD
    BOOKED

BookingStatus:
    CREATED
    CONFIRMED
    FAILED
```

---

## 2. SeatState

Implement:

```go
type SeatState struct {
    status   SeatStatus
    owner    string
    heldAt   time.Time
    mu       sync.Mutex
}
```

Then implement synchronized transitions.

---

## 3. ShowSeat

Implement:

```go
type ShowSeat struct {
    seats map[string]*SeatState
}
```

Then:

```go
Hold()
Release()
Confirm()
```

This is the heart of the problem.

---

## 4. Multi-seat atomic acquisition

Implement:

```text
sort seats
      ↓
acquire sequentially
      ↓
failure?
  /       \
yes       no
 |         |
rollback   success
```

Explain that sorting gives a global lock ordering and prevents deadlock.

---

## 5. BookingService

Implement:

```text
CreateBooking
ConfirmBooking
CancelBooking
```

Do not overcomplicate this layer.

---

## 6. Payment mock

```go
type PaymentService interface {
    Pay(amount int) bool
}
```

Use a fake implementation for the interview.

---

## 7. Expiry

Only after the basic booking flow works.

Possible implementation:

```text
ticker
  |
  v
scan HELD seats
  |
  v
expire stale holds
```

If scale is discussed, mention that a production system may use delayed jobs, expiration records, or database TTL-like mechanisms rather than scanning every seat.

---

## 8. Tests

Immediately demonstrate:

### Test 1

```text
A -> s1
B -> s1
```

Expected:

```text
A succeeds
B fails
```

### Test 2

```text
A -> s1,s2
B -> s1,s2
```

Expected:

```text
one succeeds
one fails
```

### Test 3

```text
A -> s1,s2
s1 available
s2 unavailable
```

Expected:

```text
A fails
s1 is released
```

### Test 4

```text
A holds s1
wait timeout
B books s1
```

Expected:

```text
A cannot confirm
B can acquire
```

### Test 5

```text
A -> s1
B -> s2
```

Expected:

```text
both can proceed concurrently
```

---

# What to say while coding

A strong concise explanation is:

> "The concurrency boundary is the ShowSeat inventory. I don't want one global lock for the entire show because users booking different seats should proceed concurrently. I'll use per-seat synchronization. For multiple seats, I'll acquire locks in a deterministic sorted order and roll back everything if any seat cannot be acquired. A seat moves from AVAILABLE to HELD to BOOKED, and expiry only applies to HELD. Confirmation must atomically validate ownership and expiry before moving HELD to BOOKED."

That explanation covers most of the important concurrency reasoning without requiring a huge system.

---

# What NOT to implement unless asked

Do not spend interview time implementing:

- Theater search
- Movie search
- User authentication
- REST controllers
- Database repositories
- Payment gateway internals
- Email/SMS
- Redis
- Kafka
- Microservices
- UI
- Recommendations

The interviewer can see your concurrency skill much more clearly from:

```text
ShowSeat
 + SeatState
 + Hold
 + Confirm
 + Release
 + expiry
 + concurrent tests
```
