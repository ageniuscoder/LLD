package main

import (
	"fmt"
	"sync"
)

type DataProcessor struct {
	mu sync.Mutex
	cond *sync.Cond
	dataReady bool
	data string
}

func NewDataProcessor() *DataProcessor{
	dp:=&DataProcessor{}
	dp.cond=sync.NewCond(&dp.mu)
	return dp
}

func (dp *DataProcessor) waitData() string{
	dp.mu.Lock()
	defer dp.mu.Unlock()

	for !dp.dataReady{
		dp.cond.Wait()
	}

	dp.dataReady=false
	return dp.data
}

func (dp *DataProcessor) producer(data string){
	dp.mu.Lock()
	defer dp.mu.Unlock()
	dp.data=data
	dp.dataReady=true
	dp.cond.Signal()
}

func main() {
	dp:=NewDataProcessor()
	var wg sync.WaitGroup
	wg.Add(2)
	go func(){
		defer wg.Done()
		data:=dp.waitData()
		fmt.Println("Recieved data: ",data)
	}()

	go func(){
		defer wg.Done()
		dp.producer("mangal")
	}()
	wg.Wait()
}