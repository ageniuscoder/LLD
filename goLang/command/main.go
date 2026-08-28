package main

import (
	"command/appl"
	"command/command"
	"command/remote"
)

func main() {
	light := appl.NewLight()
	fan := appl.NewFan()

	lc := command.NewLightCommand(light)
	fc := command.NewFanCommand(fan)

	remote := remote.NewRemote(2)
	remote.SetButton(0, lc)
	remote.SetButton(1, fc)

	remote.Execute(0)
	remote.Execute(0)
	remote.Execute(1)
	remote.Execute(1)
	remote.Execute(1)
}
