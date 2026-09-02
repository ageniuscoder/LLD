package main

import "sync"

type Semaphore struct {
	permits int
	mu      sync.Mutex
	full    *sync.Cond
}

func NewSemaphore(p int) *Semaphore {
	s := &Semaphore{
		permits: p,
	}
	s.full = sync.NewCond(&s.mu)
	return s
}

func (s *Semaphore) Accquire() {
	s.mu.Lock()
	defer s.mu.Unlock()

	for s.permits == 0 {
		s.full.Wait()
	}

	s.permits--
}

func (s *Semaphore) Release() {
	s.mu.Lock()
	defer s.mu.Unlock()

	s.permits++
	s.full.Signal()
}

func (s *Semaphore) TryAccquire() bool {
	s.mu.Lock()
	defer s.mu.Unlock()

	if s.permits > 0 {
		s.permits--
		return true
	}
	return false
}
