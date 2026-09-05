package main

import "mediator/mediator"

func main() {
	med := mediator.NewChatMediator()

	u1 := mediator.NewUser("mangal", med)
	u2 := mediator.NewUser("rahul", med)
	u3 := mediator.NewUser("manya", med)

	u1.SendTo(u2, "Hello Rahul")

	u3.SendTo(u1, "kya kr rhe ho mangal")

	u2.Broadcast("Hey everyone")
}
