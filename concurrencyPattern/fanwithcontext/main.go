package main

import (
	"context"
	"fmt"
	"sync"
)

func gen(ctx context.Context, nums ...int) <-chan int {
	out := make(chan int)
	go func() {
		defer close(out)
		for _, val := range nums {
			select{
			case out<-val:
			case <-ctx.Done():
				return 
			}
		}
	}()
	return out
}
func sq(ctx context.Context,in <-chan int) <-chan int {
	out := make(chan int)
	go func() {
		defer close(out)
		for{
			select{
			case val,ok:=<-in:
				if !ok{
					return
				}
				select{
				case out<-val*val:
				case <-ctx.Done():
					return 
				}
			case <-ctx.Done():
				return 
			}
		}
	}()
	return out
}
func merge(ctx context.Context,channels ...<-chan int) <-chan int{
	out:=make(chan int)
	var wg sync.WaitGroup
	wg.Add(len(channels))
	for _,ch:=range channels{
		go func(c <-chan int){
			defer wg.Done()
			for{
				select{
				case val,ok:=<-c:
					if !ok{
						return
					}
					select{
					case out<-val:
					case <-ctx.Done():
						return
					}
				case <-ctx.Done():
					return 
				}
			}
		}(ch)
	}
	go func(){
		wg.Wait()
		close(out)
	}()
	
	return out
}
func main() {
	ctx,cancel:=context.WithCancel(context.Background())
	defer cancel()

	jobs:=gen(ctx,1,2,3,4)
	w1:=sq(ctx,jobs)
	w2:=sq(ctx,jobs)
	result:=merge(ctx,w1,w2)

	for r:=range result{
		if r>10{
			cancel()
			break
		}
		fmt.Println(r)
	}
}