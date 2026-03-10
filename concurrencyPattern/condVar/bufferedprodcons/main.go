/*The bounded buffer is the classic condition variable example, and for good reason: it elegantly demonstrates why you might need multiple conditions. A bounded buffer is a fixed-size queue where producers add items and consumers remove them.

The interesting constraint: producers must wait when the buffer is full, and consumers must wait when it's empty. This bidirectional waiting can't be elegantly solved with a single condition variable. You need two: one for "buffer has space" and one for "buffer has items."*/

package main

import (
	"fmt"
	"sync"
)

type boundedBuffer struct{
	mu sync.Mutex
	hasItem *sync.Cond
	NotEmpty *sync.Cond
	buffer []interface{}
	capacity int
}

func NewBoundedBuffer(cap int) *boundedBuffer{
	bb:=&boundedBuffer{
		buffer: make([]interface{},0,cap),
		capacity: cap,
	}
	bb.hasItem=sync.NewCond(&bb.mu)
	bb.NotEmpty=sync.NewCond(&bb.mu)
	return bb
}

func (bb *boundedBuffer) Put(item interface{}){
	bb.mu.Lock()
	defer bb.mu.Unlock()

	for len(bb.buffer)==bb.capacity{
		bb.NotEmpty.Wait()
	}

	bb.buffer=append(bb.buffer, item)
	bb.hasItem.Signal()

}

func (bb *boundedBuffer) Take() interface{}{
	bb.mu.Lock()
	defer bb.mu.Unlock()

	for len(bb.buffer)==0{
		bb.hasItem.Wait()
	}

	item:=bb.buffer[0]
	bb.buffer=bb.buffer[1:]
	bb.NotEmpty.Signal()
	return item
}

func main(){
	bb:=NewBoundedBuffer(5)

	var wg sync.WaitGroup
	wg.Add(2)

	go func(){
		defer wg.Done()
		for i:=0;i<10;i++{
			bb.Put(i)
		}
	}()
	go func(){
		defer wg.Done()
		for i:=0;i<10;i++{
			fmt.Println("Recived data",bb.Take())
		}
	}()
	wg.Wait()
}