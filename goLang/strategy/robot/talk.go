package robot

import "fmt"

type Talker interface {
	Talk()
}

type NormalTalk struct{}

func (n NormalTalk) Talk() {
	fmt.Println("Normal Talk")
}
type NoTalk struct{}
func (n NoTalk) Talk(){
	fmt.Println("No Talk")
}