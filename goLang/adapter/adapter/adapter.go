package adapter

import "fmt"

type ThirdParty struct {
}

func NewThirdParty() *ThirdParty {
	return &ThirdParty{}
}

func (t *ThirdParty) d() {
	fmt.Println("Third party is invoked")
}

type Mangal interface {
	C()
}

type test struct {
}

func (t test) C() {
	fmt.Println("wants to communicate third party")
}

type Adapter struct {
	t *ThirdParty
}

func NewAdapter(t *ThirdParty) Mangal {
	return Adapter{
		t,
	}
}

func (a Adapter) C() {
	a.t.d()
}
