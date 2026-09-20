package heart

import (
	"bookmyshow/enum"
	"bookmyshow/lockprovider"
	"slices"
	"sync"
	"time"
)

type ShowSeat struct {
	timeout time.Duration
	seats   map[string]*SeatState
}

func NewShowSeat(screen *Screen, timeout time.Duration) *ShowSeat {
	seats := make(map[string]*SeatState)
	for _, seat := range screen.GetSeats() {
		seats[seat.GetId()] = NewSeatState()
	}
	s := &ShowSeat{
		seats:   seats,
		timeout: timeout,
	}
	go s.runExpiryWorker()

	return s
}

func (s *ShowSeat) GetSeatStateById(id string) *SeatState {
	return s.seats[id]
}

func (s *ShowSeat) Hold(name string, seats []string) bool {
	slices.Sort(seats)
	holded := make([]string, 0)
	for _, id := range seats {
		if !s.seats[id].tryHold(name) {
			for _, ids := range holded {
				s.seats[ids].unlock()
			}
			return false
		}

		holded = append(holded, id)
	}

	return true

}

func (s *ShowSeat) runExpiryWorker() {
	ticker := time.NewTicker(2 * time.Second)
	defer ticker.Stop()

	for range ticker.C {
		for _, seat := range s.seats {
			if seat.isExpired(s.timeout) {
				seat.unlock()
			}
		}
	}
}

type SeatState struct {
	lastlocked time.Time
	lockedby   string
	status     enum.SeatStatus
	lock       lockprovider.LockProvider
	mu         sync.Mutex
}

func NewSeatState() *SeatState {
	return &SeatState{
		lastlocked: time.Time{},
		status:     enum.SEAT_AVAIL,
		lock:       lockprovider.NewSeatLock(),
	}
}

func (s *SeatState) WhoHeld() string {
	s.mu.Lock()
	defer s.mu.Unlock()
	return s.lockedby
}

func (s *SeatState) setHeld(name string) {
	s.mu.Lock()
	defer s.mu.Unlock()
	s.lastlocked = time.Now()
	s.lockedby = name
	s.status = enum.SEAT_HELD
}

func (s *SeatState) isExpired(timeout time.Duration) bool {
	s.mu.Lock()
	expired := (s.status == enum.SEAT_HELD && time.Since(s.lastlocked) > timeout)
	s.mu.Unlock()
	return expired
}

func (s *SeatState) unlock() {
	s.mu.Lock()
	defer s.mu.Unlock()
	s.lastlocked = time.Time{}
	s.lockedby = ""
	s.status = enum.SEAT_AVAIL
	s.lock.Unlock()
}

func (s *SeatState) MarkBooked() {
	s.mu.Lock()
	defer s.mu.Unlock()
	s.status = enum.SEAT_BOOKED
}

func (s *SeatState) tryHold(name string) bool {
	if !s.lock.TryLock() {
		return false
	}
	s.setHeld(name)
	return true
}
