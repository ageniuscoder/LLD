package main

import "itert/ll"

func main() {

	ll := ll.NewLl[int]()
	ll.InsertEnd(5)
	ll.InsertEnd(6)
	ll.InsertEnd(7)

	it := ll.GetIterator()

	for it.IsNext() {
		println(it.Next())
	}

	println("Iterator pattern")
}
