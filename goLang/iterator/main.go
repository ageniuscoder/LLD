package main

import (
	"fmt"
	"itert/ll"
	"itert/song"
	"time"
)

func main() {

	ll := ll.NewLl[int]()
	ll.InsertEnd(5)
	ll.InsertEnd(6)
	ll.InsertEnd(7)

	it := ll.GetIterator()

	for it.IsNext() {
		println(it.Next())
	}

	s1 := song.NewSong("dildar", 160*time.Second)
	s2 := song.NewSong("beliya", 125*time.Second)

	pl := song.NewPlaylist("mangal-p")
	pl.AddSong(s1)
	pl.AddSong(s2)

	it1 := pl.GetIterator()

	for it1.IsNext() {
		fmt.Printf("%+v\n", it1.Next())
	}

	println("Iterator pattern")
}
