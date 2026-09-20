# Concurrent Seat Booking — Interview Scope & Assumptions

## Interview goal

The interviewer is primarily checking whether the candidate can correctly implement **concurrent seat booking for a show**.

The design should therefore prioritize:

1. Correct seat ownership under concurrency.
2. Temporary seat holds.
3. Hold expiry.
4. Atomic multi-seat hold.
5. Successful payment/confirmation.
6. Rejection of stale/expired holds.
7. No double booking.
8. Reasonable deadlock avoidance.
9. Clear separation of responsibilities.

Do **not** spend most of the interview designing every BookMyShow feature.

---

## Core assumption

A `Show` owns an independent seat inventory.

For example:

```text
Movie: Interstellar
Theater: PVR
Screen: Screen-1
Show: 7:00 PM

ShowSeat
  s-11 -> AVAILABLE
  s-12 -> AVAILABLE
  s-13 -> BOOKED
  ...
```

The same physical screen can have multiple shows, but each show has its own `ShowSeat` state.

Therefore:

```text
Screen
  └── physical seats

Show
  └── ShowSeat
       ├── s-11
       ├── s-12
       └── ...
```

A seat being booked for the 7 PM show must not affect the same physical seat for the 10 PM show.

---

## Seat state machine

```text
AVAILABLE
    |
    | hold(user)
    v
HELD
    |
    +--------------------+
    |                    |
    | confirm            | timeout / release
    v                    v
BOOKED                AVAILABLE
```

Important invariant:

> A `BOOKED` seat must never become available again during the lifetime of that show.

---

## Hold assumptions

- A hold is temporary.
- Example hold duration: 2–5 minutes.
- Only the user who owns the hold can confirm it.
- An expired hold cannot be confirmed.
- If any requested seat cannot be held, the entire multi-seat hold fails.
- Seats already held or booked cannot be acquired.

---

## Concurrency assumptions

Multiple users may simultaneously request:

```text
User A -> s-11
User B -> s-11
```

Only one request can succeed.

Multiple users may also request:

```text
User A -> [s-11, s-12]
User B -> [s-11, s-12]
```

Only one request can successfully acquire both.

Different seats should be able to proceed concurrently:

```text
User A -> s-11
User B -> s-12
```

There is no need to serialize the entire show if seat-level locking is used.

---

## Payment assumption

For an interview, payment can be represented by:

```go
func Pay(amount int) bool
```

The payment provider itself does not need to be implemented.

The important requirement is:

> Payment success must result in confirmation only if the user's seat hold is still valid.

---

## Out of scope unless interviewer asks

Do not initially design:

- Search/ranking
- Recommendations
- Reviews
- Coupons
- Wallet
- Real payment gateway internals
- Notifications
- Distributed databases
- Kafka
- Redis
- Microservices
- Multi-region deployment
- Dynamic pricing
- Analytics

If the interviewer asks about distributed deployment, then discuss how the seat lock would move from process-local synchronization to a distributed/transactional mechanism.
