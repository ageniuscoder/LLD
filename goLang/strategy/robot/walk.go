package robot

import "fmt"

type Walker interface {
	Walk()
}

type NormalWalk struct{}
func (n NormalWalk) Walk(){
	fmt.Println("normal Walk")
}
type NoWalk struct{}
func (n NoWalk) Walk(){
	fmt.Println("no walk")
}