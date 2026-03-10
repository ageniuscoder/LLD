package main

import (
	"fmt"
	"sync"
	"time"
)

type signalPattern struct {
	singal chan struct{}
	data   string
}

func NewSignalPattern() *signalPattern {
	return &signalPattern{
		singal: make(chan struct{}),
	}
}

func (sp *signalPattern) signalar() {
	fmt.Println("[signalar]: preparing data for waiter")
	time.Sleep(500*time.Millisecond)

	sp.data="important data"
	fmt.Println("[signalar]: data is prepared,, sending signal")
	sp.singal<-struct{}{}

	fmt.Println("[signalar]: continuing other work")
}

func (sp *signalPattern) waiter(){
	fmt.Println("[waiter]: waiting for signal")

	<-sp.singal
	fmt.Println("[waiter]: signal recieved")

	fmt.Println("[waiter]: working on data",sp.data)
}

func main() {
	demo:=NewSignalPattern()

	var wg sync.WaitGroup
	wg.Add(2)
	go func(){
		defer wg.Done()
		demo.waiter()
	}()

	time.Sleep(500*time.Millisecond)  //time given for blocking the waiter

	go func(){
		defer wg.Done()
		demo.signalar()
	}()

	wg.Wait()
}