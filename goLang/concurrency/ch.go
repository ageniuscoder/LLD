// package main

// import "fmt"

// func main() {
// 	// ch := make(chan int, 3)

// 	// end:=make(chan bool)

// 	// go func(){
// 	// 	ch <- 1
// 	// 	ch <- 2
// 	// 	ch <- 3

// 	// 	ch <- 4

// 	// 	//ch <- 5

// 	// 	end<-true
// 	// }()

// 	// val:=<-ch
// 	// // val1:=<-ch
// 	// // val2:=<-ch
// 	// // val3:=<-ch

// 	// fmt.Println(val)
// 	// // fmt.Println(val2)
// 	// // fmt.Println(val3)
// 	// // fmt.Println(val)

// 	// fmt.Println("end of program")

// 	// <-end

// 	ch := make(chan int)


// 	go func() {
// 		for i := 0; i < 4; i++ {
// 			ch <- i
// 		}
// 		close(ch)
// 	}()

// 	for val:=range ch{
// 		fmt.Println(val)
// 	}


// 	fmt.Println("program end")

// }

// // for value := range ch {             //loop is used to receive values from a channel until the channel is closed.
// //     fmt.Println(value)
// // }