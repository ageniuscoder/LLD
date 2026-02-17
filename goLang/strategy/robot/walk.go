package robot

import "fmt"

type Walker interface {
	Walk()
}

type normalWalk struct{}
func NewNormalWalk() *normalWalk{
	return &normalWalk{}
}
func (n normalWalk) Walk(){
	fmt.Println("normal Walk")
}
type noWalk struct{}
func NewNoWalk() *noWalk{
	return &noWalk{}
}
func (n noWalk) Walk(){
	fmt.Println("no walk")
}