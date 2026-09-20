package lockprovider

import "sync"

type LockProvider interface {
	Lock()
	Unlock()
	TryLock() bool
}

type SeatLock struct {
	mu sync.Mutex
}

func NewSeatLock() *SeatLock {
	return &SeatLock{}
}

func (s *SeatLock) Lock() {
	s.mu.Lock()

}

func (s *SeatLock) Unlock() {
	s.mu.Unlock()
}

func (s *SeatLock) TryLock() bool {
	return s.mu.TryLock()
}
