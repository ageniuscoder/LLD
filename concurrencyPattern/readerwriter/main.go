//reader preferenced rw lock

package main

import (
	"fmt"
	"sync"
	"time"
)

type RWLock struct {
	mu     sync.Mutex
	cond   *sync.Cond
	reader int
	writer bool
}

func NewRWLock() *RWLock {
	rw := &RWLock{}
	rw.cond = sync.NewCond(&rw.mu)
	return rw
}

func (rw *RWLock) Rlock() {
	rw.mu.Lock()
	defer rw.mu.Unlock()
	for rw.writer {
		rw.cond.Wait()
	}
	rw.reader++
}

func (rw *RWLock) Runlock() {
	rw.mu.Lock()
	defer rw.mu.Unlock()

	rw.reader--
	if rw.reader == 0 {
		rw.cond.Broadcast()
	}
}

func (rw *RWLock) Wlock() {
	rw.mu.Lock()
	defer rw.mu.Unlock()

	for rw.reader > 0 || rw.writer {
		rw.cond.Wait()
	}

	rw.writer = true
}

func (rw *RWLock) Wunlock() {
	rw.mu.Lock()
	defer rw.mu.Unlock()

	rw.writer = false
	rw.cond.Broadcast()

}
func main() {
	lock := NewRWLock()
	var wg sync.WaitGroup

	// Start 5 readers
	for i := 1; i <= 10; i++ {
		wg.Add(1)
		go func(id int) {
			defer wg.Done()
			lock.Rlock()
			fmt.Printf("Reader %d acquired lock\n", id)
			time.Sleep(3 * time.Nanosecond)
			fmt.Printf("Reader %d releasing lock\n", id)
			lock.Runlock()
		}(i)
	}

	// Start 1 writer
	wg.Add(1)
	go func() {
		defer wg.Done()
		lock.Wlock()
		fmt.Println("Writer acquired lock")
		time.Sleep(100 * time.Millisecond)
		fmt.Println("Writer releasing lock")
		lock.Wunlock()
	}()

	wg.Wait()
}
