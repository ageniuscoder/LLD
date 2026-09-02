//buffered chan can be implemented using two conditional variables in golang

package main

import (
	"fmt"
	"sync"
)

type BufChan struct {
	mu       sync.Mutex
	buffer   []any
	capacity int
	full     *sync.Cond //when full producer has to wait
	empty    *sync.Cond //when empty consumer has to wait
}

func NewBufChan(cap int) *BufChan {
	bf := &BufChan{
		buffer:   make([]any, 0, cap),
		capacity: cap,
	}
	bf.full = sync.NewCond(&bf.mu)
	bf.empty = sync.NewCond(&bf.mu)
	return bf
}

func (bc *BufChan) Put(val any) {
	bc.mu.Lock()
	defer bc.mu.Unlock()

	for len(bc.buffer) == bc.capacity {
		bc.full.Wait()
		fmt.Println("can,t add value producer is waiting for consumer to consume")
	}

	bc.buffer = append(bc.buffer, val)
	fmt.Printf("producer -> %d \n", val)
	bc.empty.Signal()
}

func (bc *BufChan) Get() any {
	bc.mu.Lock()
	defer bc.mu.Unlock()

	for len(bc.buffer) == 0 {
		bc.empty.Wait()
		fmt.Println("buffer is empty, consumer is waiting for producer for putting values")
	}
	val := bc.buffer[0]
	bc.buffer = bc.buffer[1:]
	bc.full.Signal()
	return val
}

func main() {
	ch := NewBufChan(5)

	var wg sync.WaitGroup
	wg.Add(2)
	go func() {
		defer wg.Done()
		for i := 0; i < 10; i++ {
			ch.Put(i)
		}
	}()

	go func() {
		defer wg.Done()
		for i := 0; i < 6; i++ {
			fmt.Printf("Consumer consumed -> %d \n", ch.Get())
		}

	}()

	wg.Wait()
}
