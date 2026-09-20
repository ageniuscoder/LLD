# Core Domain Model

The minimum useful domain model is:

```text
Movie
Screen
Seat
Show
ShowSeat
LockProvider
SeatState
Booking
Payment
BookingService
```

---

## Movie

```go
type Movie struct {
    id       string
    name     string
    duration time.Duration
}
```

Only basic information is required.

---

## Screen

A screen owns the **physical seats**.

```go
type Screen struct {
    id    string
    seats []Seat
}
```

Example:

```text
Screen-1

A1 A2 A3 A4 A5
B1 B2 B3 B4 B5
C1 C2 C3 C4 C5
```

---

## Show

A show connects a movie with a screen at a particular time.

```go
type Show struct {
    id        string
    movie     Movie
    screen    *Screen
    startTime time.Time
    seats     *ShowSeat
}
```

`ShowSeat` is the important object for concurrency.

---

## Seat

Physical seat information:

```go
type Seat struct {
    id  string
    row int
    pos int
}
```

It does NOT contain booking state.

---

## ShowSeat

`ShowSeat` maintains the state of every physical seat for one show.

```go
type ShowSeat struct {
    seats map[string]*SeatState
    // synchronization
}
```

Example:

```text
Show 7 PM

ShowSeat
  |
  +-- s-11 -> SeatState
  +-- s-12 -> SeatState
  +-- s-13 -> SeatState
```

This is where concurrent booking is controlled.

---

## LockProvider

`LockProvider` provide lock for a single show seat.

```go
type LockProvider interface {
   Lock()
   Unlock()
   TryLock()
}
```

---

## SeatState

```go
type SeatState struct {
    status     SeatStatus
    lockedBy   string
    lockedAt   time.Time
}
```

Possible states:

```go
AVAILABLE
HELD
BOOKED
```

---

## Booking

```go
type Booking struct {
    id     string
    user   string
    show   *Show
    seats  []string
    status BookingStatus
    payment *Payment
}
```

A booking represents the user's attempt to purchase seats.

---

## Payment

```go
type Payment struct {
    amount int
    status PaymentStatus
}
```

Payment gateway implementation is outside the core LLD.

---

# Most important ownership rule

The concurrency state belongs to:

```text
Show -> ShowSeat -> SeatState
```

not:

```text
Screen -> Seat
```

because the same screen can have different seat availability for different shows.
