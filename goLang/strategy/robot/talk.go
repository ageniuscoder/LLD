package robot

import "fmt"

type Talker interface {
	Talk()
}

type normalTalk struct{}

func NewNormalTalk() *normalTalk{
	return &normalTalk{}
}

func (n normalTalk) Talk() {
	fmt.Println("Normal Talk")
}
type noTalk struct{}

func NewNoTalk() *noTalk{
	return &noTalk{}
}
func (n noTalk) Talk(){
	fmt.Println("No Talk")
}