// fan out and fan in pattern
package main

import (
	"fmt"
	"sync"
)

//jobs
func gen(nums ...int) <-chan int {
	out := make(chan int)
	go func() {
		for _, val := range nums {
			out <- val
		}
		close(out)
	}()
	return out
}
//workers 
func sq(in <-chan int) <-chan int {
	out := make(chan int)
	go func() {
		for val := range in {
			out <- val * val
		}
		close(out)
	}()
	return out
}
//fan-in   using waitgroups
func merge(channels ...<-chan int) <-chan int{
	out:=make(chan int)
	var wg sync.WaitGroup
	wg.Add(len(channels))
	for _,ch:=range channels{
		go func(c <-chan int){
			defer wg.Done()
			for val:=range c{
				out<-val
			}
		}(ch)
	}
	go func(){
		wg.Wait()
		close(out)
	}()
	
	return out
}
//fan-in   using channels as waitgroups
func merge1(channels ...<-chan int) <-chan int{
	out:=make(chan int)
	done:=make(chan struct{})
	for _,ch:=range channels{
		go func(c <-chan int){
			for val:=range c{
				out<-val
			}
			done<-struct{}{}
		}(ch)
	}
	go func(){
		for i:=0;i<len(channels);i++{
			<-done
		}
		close(out)
	}()
	return out
}

//consumer
func consumer(in <-chan int) []int{
	res:=make([]int,0)
	for val:=range in{
		res=append(res, val)
	}
	return res
}
func main() {
	jobs:=gen(1,2,3,4,5)

	//fan-out
	w1:=sq(jobs)
	w2:=sq(jobs)
	w3:=sq(jobs)

	result:=merge1(w1,w2,w3)

	fmt.Println(consumer(result))

}