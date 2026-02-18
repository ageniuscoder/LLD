package main

import (
	item "composite/items"
	"fmt"
)

func main(){
	root:=item.NewFolder("root")
	root.Add(item.NewFile("f1.txt",4))
	root.Add(item.NewFile("f2.txt",6))
	music:=item.NewFolder("music")
	root.Add(music)
	music.Add(item.NewFile("s1.mp3",5))

	fmt.Println("Ls command")
	root.Ls(0)
	fmt.Println("openAll command")
	root.OpenAll(0)

}