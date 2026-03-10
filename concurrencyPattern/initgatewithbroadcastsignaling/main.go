//application of signaling pattern

package main

import (
	"fmt"
	"sync"
	"time"
)

type initGate struct{
	gate chan struct{}
	init bool
	numWorkers int
}

func NewInitGate(numWorkers int) *initGate{
	return &initGate{
		gate: make(chan struct{},numWorkers),
		init: false,
		numWorkers: numWorkers,
	}
}

func (ig *initGate) Init(){
	fmt.Println("Main: Starting initialization...")

	// Simulate initialization work
	time.Sleep(200 * time.Millisecond)

	ig.init = true
	fmt.Println("Main: Initialization complete, opening gate")

	// for i:=0;i<ig.numWorkers;i++{  it is sending signal to each worker
	// 	ig.gate<-struct{}{}
	// }

	close(ig.gate)   //this is broadcast signaling

}

func (ig *initGate) workers(id int,wg *sync.WaitGroup){
	defer wg.Done()
	fmt.Printf("Worker %d: Waiting at gate...\n", id)

	<-ig.gate

	fmt.Printf("Worker %d: Gate opened, starting work\n", id)
}
func main(){
	n:=5
	var wg sync.WaitGroup

	gate:=NewInitGate(n)

	for i:=1;i<=n;i++{
		wg.Add(1)
		go gate.workers(i,&wg)
	}

	time.Sleep(500*time.Millisecond)

	gate.Init()

	wg.Wait()
}