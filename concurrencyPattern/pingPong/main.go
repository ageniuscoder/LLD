//application of signaling

package main

import (
	"fmt"
	"sync"
)
type pingPong struct{
	n int
	pic chan struct{}
	poc chan struct{}
}

func NewPingPong(n int) *pingPong{
	p:=pingPong{
		n:n,
		pic: make(chan struct{},1),
		poc: make(chan struct{},1),
	}
	p.poc<-struct{}{}
	return &p
}

func (p *pingPong) ping(wg *sync.WaitGroup){
	defer wg.Done()
	fmt.Println("[Ping]: waiting for pong signal")
	for i:=1;i<=p.n;i++{
		<-p.poc
		fmt.Println("[Ping]: signal recieved from pong")
		fmt.Println("PING")

		p.pic<-struct{}{}
		fmt.Println("[ping]: siganl sent for pong")
	}

}

func (p *pingPong) pong(wg *sync.WaitGroup){
	defer wg.Done()
	fmt.Println("[Pong]: waiting for ping signal")
	for i:=1;i<=p.n;i++{
		<-p.pic
	    fmt.Println("[Pong]: signal recieved from ping")
		fmt.Println("PONG")

		p.poc<-struct{}{}
		fmt.Println("[pong]: siganl sent for ping")
	}
}
func main(){
	n:=5
	var wg sync.WaitGroup
	pp:=NewPingPong(n)

	wg.Add(2)

	go func(){
		pp.ping(&wg)
	}()

	go func(){
		pp.pong(&wg)
	}()

	

	wg.Wait()
	fmt.Println("executed succesfully")
}