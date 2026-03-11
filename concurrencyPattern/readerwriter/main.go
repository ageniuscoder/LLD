//reader preferenced rw lock

package main

import "sync"

type RWLock struct{
	mu sync.Mutex
	cond *sync.Cond
	reader int
	writer int
	readerreq int
}

func NewRWLock() *RWLock{
	rw:=&RWLock{}
	rw.cond=sync.NewCond(&rw.mu)
	return rw
}

func (rw *RWLock) Rlock(){
	rw.mu.Lock()
	defer rw.mu.Unlock()
	rw.readerreq++
	for rw.writer>0 {
		rw.cond.Wait()
	}
	rw.reader++
	rw.readerreq--
}

func (rw *RWLock) Runlock(){
	rw.mu.Lock()
	defer rw.mu.Unlock()

	rw.reader--
	if rw.reader==0{
		rw.cond.Signal()
	}
}

func (rw *RWLock) Wlock(){
	rw.mu.Lock()
	defer rw.mu.Unlock()

	for rw.reader>0 || rw.writer>0 || rw.readerreq>0{
		rw.cond.Wait()
	}

	rw.writer++
}

func (rw *RWLock) Wunlock(){
	rw.mu.Lock()
	defer rw.mu.Unlock()

	rw.writer--

	if rw.writer==0{
		rw.cond.Broadcast()
	}
}
func main() {}