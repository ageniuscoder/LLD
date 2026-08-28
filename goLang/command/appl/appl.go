package appl

import "fmt"

type Light struct {
}

func NewLight() Light {
	return Light{}
}

func (l Light) On() {
	fmt.Println("Light is On")
}

func (l Light) Off() {
	fmt.Println("Light is Off")
}

type Fan struct {
}

func NewFan() Fan {
	return Fan{}
}

func (l Fan) On() {
	fmt.Println("Fan is On")
}

func (l Fan) Off() {
	fmt.Println("Fan is Off")
}
