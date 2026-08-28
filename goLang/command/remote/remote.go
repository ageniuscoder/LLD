package remote

import (
	"command/command"
)

type Remote struct {
	n       int
	buttons []command.Command
}

func NewRemote(n int) *Remote {
	return &Remote{
		n:       n,
		buttons: make([]command.Command, n),
	}
}

func (r *Remote) SetButton(idx int, cmd command.Command) {
	r.buttons[idx] = cmd
}

func (r *Remote) Execute(idx int) {
	r.buttons[idx].Execute()
}
