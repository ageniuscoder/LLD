package command

import "command/appl"

type Command interface {
	Execute()
}

type LightCommand struct {
	isOn  bool
	light appl.Light
}

func NewLightCommand(light appl.Light) Command {
	return &LightCommand{
		isOn:  false,
		light: light,
	}
}

func (l *LightCommand) Execute() {
	if l.isOn {
		l.light.Off()
	} else {
		l.light.On()
	}
	l.isOn = !l.isOn
}

type FanCommand struct {
	isOn bool
	fan  appl.Fan
}

func NewFanCommand(fan appl.Fan) Command {
	return &FanCommand{
		isOn: false,
		fan:  fan,
	}
}

func (l *FanCommand) Execute() {
	if l.isOn {
		l.fan.Off()
	} else {
		l.fan.On()
	}
	l.isOn = !l.isOn
}
