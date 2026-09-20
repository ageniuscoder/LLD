# Complete Booking Flow

## High-level flow

```text
User
 |
 | select seats
 v
App / BookingService
 |
 | Hold(show, user, seats)
 v
ShowSeat
 |
 | all seats available?
 |
 +---- NO ----> booking failed
 |
 YES
 |
 v
Seats = HELD
 |
 v
Create Booking
 |
 v
Payment
 |
 +---- FAILED ----> Release held seats
 |
 SUCCESS
 |
 v
Confirm Booking
 |
 | validate ownership + expiry
 v
Seats = BOOKED
 |
 v
Booking = CONFIRMED
```

---

# Step 1 — User requests seats

Example:

```go
Book(
    user="Alice",
    show=show,
    seats=["s-11", "s-12"],
)
```

---

# Step 2 — Hold seats

`BookingService` asks:

```go
show.ShowSeats.Hold("Alice", []string{"s-11", "s-12"})
```

`ShowSeat`:

1. Validates seat IDs.
2. Sorts them.
3. Attempts to acquire each seat.
4. Marks each acquired seat as HELD.
5. Records:
   - owner
   - hold time

If one seat fails:

```text
s-11 -> acquired
s-12 -> acquired
s-13 -> failed
```

then:

```text
release s-11
release s-12
```

and return failure.

---

# Step 3 — Create booking

Only after the hold succeeds:

```go
booking := NewBooking(...)
```

Status:

```text
BOOKING_CREATED
```

---

# Step 4 — Payment

Simplified interview assumption:

```go
success := paymentService.Pay(amount)
```

Real gateway internals are out of scope.

---

# Step 5 — Payment failure

If payment fails:

```text
Booking -> FAILED
ShowSeat -> release seats
```

The seats become:

```text
AVAILABLE
```

---

# Step 6 — Payment success

Do not simply do:

```go
WhoHeld()
MarkBooked()
```

as two unrelated operations.

Instead use one logical operation:

```go
Confirm(user, seatIDs)
```

which validates:

```text
seat status == HELD
owner == user
hold has not expired
```

and changes:

```text
HELD -> BOOKED
```

as one synchronized transition.

---

# Step 7 — Successful confirmation

Final state:

```text
SeatState:
    status = BOOKED
    owner = optional historical owner
    hold timer no longer applies

Booking:
    status = CONFIRMED
```

A BOOKED seat must not be unlocked.

---

# Expiry flow

A held seat has:

```text
lockedAt
holdTimeout
```

A background cleanup mechanism can periodically inspect held seats.

If:

```text
now - lockedAt > timeout
```

then:

```text
HELD -> AVAILABLE
```

and the booking/hold becomes invalid.

---

# Expiry vs payment

This is the critical concurrency case.

Potential interleaving:

```text
Alice                    Expiry worker

payment confirmation
       |
       | validate hold
       |
       |-----------------> expire?
       |
       v
confirm
```

The implementation must guarantee that an expired hold cannot become booked.

Therefore the validation and transition should happen under the same synchronization boundary.

---

# Recommended responsibility

Prefer:

```go
ShowSeat.Hold(...)
ShowSeat.Confirm(...)
ShowSeat.Release(...)
```

instead of having `App` manipulate individual `SeatState`s.

`ShowSeat` owns the seat inventory and therefore owns its concurrency rules.
