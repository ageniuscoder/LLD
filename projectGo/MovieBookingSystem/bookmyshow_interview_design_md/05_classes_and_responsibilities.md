# Classes and Responsibilities

## App

Coordinates the use case.

```text
App
 |
 +-- BookingService
```

Responsibilities:

- receive booking request
- invoke booking service
- return result

It should NOT contain seat-locking logic.

---

## BookingService

Responsibilities:

- create booking
- invoke seat hold
- invoke payment
- invoke confirmation
- update booking state

It should NOT directly manipulate mutexes.

---

## ShowSeat

This is the most important concurrency class.

Responsibilities:

- maintain seat state for one show
- hold seats
- release seats
- confirm seats
- enforce concurrency rules
- manage hold expiry

Key API:

```go
Hold(user string, seats []string) bool
Confirm(user string, seats []string) bool
Release(user string, seats []string)
```

---

## SeatState

Responsibilities:

- represent one seat's state
- maintain owner
- maintain hold timestamp
- provide synchronized state transition

It should not know about bookings or payments.

---

## Booking

Responsibilities:

- booking identity
- user
- show
- selected seats
- booking status
- payment information

---

## PaymentService

For the interview:

```go
Pay(amount int) bool
```

The implementation can be mocked.

---

# Dependency direction

Recommended:

```text
App
 |
 v
BookingService
 |
 +------> Show
 |          |
 |          v
 |       ShowSeat
 |
 +------> PaymentService
 |
 v
Booking
```

Concurrency should stay close to the resource being protected:

```text
ShowSeat
   |
   v
SeatState
   |
   v
Lock
```

---

# What not to put in App

Avoid:

```go
for _, seat := range booking.GetSeats() {
    seat.Lock()
    ...
}
```

The application layer should not know how seats are synchronized.

It should say:

```go
showSeat.Confirm(user, seats)
```

and let `ShowSeat` enforce its invariants.
