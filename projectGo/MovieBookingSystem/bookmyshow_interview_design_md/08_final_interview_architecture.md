# Final Interview Architecture

## Minimal architecture to present

```text
                    +----------------+
                    |      User      |
                    +-------+--------+
                            |
                            v
                    +---------------+
                    |      App      |
                    +-------+-------+
                            |
                            v
                  +-------------------+
                  |  BookingService   |
                  +----+---------+----+
                       |         |
             hold/     |         | payment
             confirm   |         |
                       v         v
               +-----------+  +-------------+
               | ShowSeat  |  |   Payment   |
               +-----+-----+  +-------------+
                     |
          +----------+----------+
          |          |          |
          v          v          v
       SeatState  SeatState  SeatState
       + lock     + lock     + lock
          |
          v
       AVAILABLE
          |
          v
         HELD
          |
       +--+--+
       |     |
    expiry  confirm
       |     |
       v     v
   AVAILABLE BOOKED
```

---

# Core APIs

```go
type ShowSeat struct {
    seats map[string]*SeatState
}

func (s *ShowSeat) Hold(
    user string,
    seatIDs []string,
) bool

func (s *ShowSeat) Confirm(
    user string,
    seatIDs []string,
) bool

func (s *ShowSeat) Release(
    user string,
    seatIDs []string,
) bool
```

---

# Interview-level booking flow

```text
1. User requests seats.

2. BookingService asks ShowSeat to hold them.

3. ShowSeat:
   - validates IDs
   - sorts IDs
   - acquires per-seat synchronization
   - verifies availability
   - marks seats HELD
   - records owner and hold timestamp

4. If any seat fails:
   - release all seats acquired by this request
   - return failure

5. Create booking.

6. Process payment.

7. If payment fails:
   - release hold
   - mark booking FAILED

8. If payment succeeds:
   - atomically validate owner and expiry
   - transition HELD -> BOOKED
   - mark booking CONFIRMED
```

---

# The three most important concurrency decisions

## 1. Per-seat locking

Prevents double booking while allowing independent seats to proceed concurrently.

## 2. Deterministic lock ordering

Prevents deadlock for multi-seat acquisition.

## 3. Atomic confirmation

Prevents:

```text
hold expires
between ownership check and booking
```

---

# What the interviewer should be able to see

If the interviewer wants to judge only:

> "Can this candidate implement concurrent seat booking correctly?"

then these components are enough:

```text
Seat
SeatState
Show
ShowSeat
Booking
BookingService
Payment mock
```

with:

```text
AVAILABLE / HELD / BOOKED
       +
per-seat synchronization
       +
atomic multi-seat hold
       +
rollback
       +
expiry
       +
atomic confirmation
       +
concurrent tests
```

Everything else is secondary.

---

# Suggested implementation target

If time is limited, prioritize this exact sequence:

```text
SeatState
   ↓
ShowSeat
   ↓
Hold()
   ↓
multi-seat rollback
   ↓
Confirm()
   ↓
BookingService
   ↓
expiry
   ↓
concurrency tests
```

Do not try to implement the complete BookMyShow product.

The interviewer is much more likely to be impressed by a small design whose concurrency invariants are correct than a large design with half-implemented features.
