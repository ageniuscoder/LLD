package mediator

import "fmt"

type Peer interface {
	SendTo(to Peer, msg string)
	Broadcast(msg string)
	recieve(from, msg string)
}

var cnt int

func nextId() string {
	cnt += 1
	return fmt.Sprintf("user-%d", cnt)
}

type User struct {
	id       string
	name     string
	mediator Mediator
}

func NewUser(name string, med Mediator) *User {
	u := &User{
		id:       nextId(),
		name:     name,
		mediator: med,
	}
	u.mediator.Register(u)
	return u
}

func (u *User) SendTo(to Peer, msg string) {
	u.mediator.SendTo(u.id, to.(*User).id, msg)
}

func (u *User) Broadcast(msg string) {
	u.mediator.Broadcast(u.id, msg)
}

func (u *User) recieve(from, msg string) {
	fmt.Printf("[%s] recieved from [%s] : %s \n", u.id, from, msg)
}
