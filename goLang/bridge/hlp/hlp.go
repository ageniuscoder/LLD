package hlp

import (
	"bridge/llp"
	"fmt"
)

type Car interface {
	Drive()
}

type car struct {
	ctype  string
	engine llp.Engine
}

func newCar(ctype string, engine llp.Engine) Car {
	return car{
		ctype,
		engine,
	}
}

func (c car) Drive() {
	fmt.Println(c.ctype + "   with  " + c.engine.Start())
}

type Suv struct {
	car
}

func NewSuv(engine llp.Engine) Car {
	return newCar("suv", engine)
}

type Sedan struct {
	car
}

func NewSedan(engine llp.Engine) Car {
	return newCar("sedan", engine)
}
