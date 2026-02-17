package main

import "strategy/robot"
func main() {
	r1:=robot.NewCompanionRobot(robot.NewNormalTalk(),robot.NewNormalWalk())
	r1.Talk()
	r1.Walk()
	r1.Project()

	r2:=robot.NewMurmurRobot(robot.NewNoTalk(),robot.NewNoWalk())
	r2.Talk()
	r2.Walk()
	r2.Project()

}