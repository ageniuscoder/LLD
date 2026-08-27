package state

import (
	"fmt"
)

type State interface {
	M1(*Context)
	M2()
	M3()
	M4()
}

type S1 struct{}

func NewS1() S1 {
	return S1{}
}

func (s S1) M1(c *Context) {
	fmt.Println("called m1 on S1, now moving to s2")
	c.moveState(c.s2)

}

func (s S1) M2() {
	fmt.Println("called m2 on S1, no state change")
}

func (s S1) M3() {
	fmt.Println("called m3 on S1, no state change")
}

func (s S1) M4() {
	fmt.Println("called m4 on S1, no state change")
}

type S2 struct{}

func NewS2() S2 {
	return S2{}
}

func (s S2) M1(c *Context) {
	fmt.Println("called m1 on S2, no state change")
}

func (s S2) M2() {
	fmt.Println("called m2 on S2, now moving to s3")
}

func (s S2) M3() {
	fmt.Println("called m3 on S2, no state change")
}

func (s S2) M4() {
	fmt.Println("called m4 on S2, no state change")
}

type S3 struct{}

func NewS3() S3 {
	return S3{}
}

func (s S3) M1(c *Context) {
	fmt.Println("called m1 on S3, no state change")
}

func (s S3) M2() {
	fmt.Println("called m2 on S3, no state change")
}

func (s S3) M3() {
	fmt.Println("called m3 on S3, now moving to s4")
}

func (s S3) M4() {
	fmt.Println("called m4 on S3, no state change")
}

type S4 struct{}

func NewS4() S4 {
	return S4{}
}

func (s S4) M1(c *Context) {
	fmt.Println("called m1 on S4, no state change")
}

func (s S4) M2() {
	fmt.Println("called m2 on S4, no state change")
}

func (s S4) M3() {
	fmt.Println("called m3 on S4, no state change")
}

func (s S4) M4() {
	fmt.Println("called m4 on S4, now moving to s1")
}
