package main

import "fmt"

func genrator(nums ...int) <-chan int { //...int it is varadaic param,, it is typed and it can take multiple typed params,,, it must be last one   ,,it bahave like slice
	out := make(chan int)
	go func() {
		for _, val := range nums {
			out <- val
		}
		close(out)
	}()
	return out
}
func square(in <-chan int) <-chan int {
	out := make(chan int)
	go func() {
		for val := range in {
			out <- val * val
		}
		close(out)
	}()
	return out
}
func double(in <-chan int) <-chan int {
	out := make(chan int)
	go func() {
		for val := range in {
			out <- 2 * val
		}
		close(out)
	}()
	return out
}
func consumer(in <-chan int) []int {
	ans := make([]int, 0)
	for val := range in {
		ans = append(ans, val)
	}
	return ans
}
func main() {
	ch1 := genrator(1, 2, 3, 4, 5)
	ch2 := square(ch1)
	ch3 := double(ch2)

	fmt.Println(consumer(ch3))

}