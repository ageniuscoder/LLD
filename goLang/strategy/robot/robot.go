package robot

import "fmt"

type Robot interface{
	Talk()
	Walk()
	Project()
}
type robot struct{
	t Talker
	w Walker
	projection string
}

func (r *robot) Talk(){
	r.t.Talk()
}
func (r *robot) Walk(){
	r.w.Walk()
}
func (r *robot) Project(){
	fmt.Println(r.projection)
}
func newRobot(t Talker,w Walker,projection string) Robot{
	return &robot{
		t:t,
		w:w,
		projection: projection,
	}
}
func NewCompanionRobot(t Talker,w Walker) Robot{
	return newRobot(t,w,"I am companion Robot")
}
func NewMurmurRobot(t Talker,w Walker) Robot{
	return newRobot(t,w,"I am murmur Robot")
}