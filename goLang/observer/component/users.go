package obs

import "fmt"

var userId int = 0

type User interface {
	Update()
}

type mangal struct {
	channel Channel
	id      int
	name    string
}

func NewMangal(name string, ch *striver) *mangal {
	userId++
	return &mangal{
		id:      userId,
		name:    name,
		channel: ch,
	}
}
func (m *mangal) Id() int {
	return m.id
}
func (m *mangal) Update() {
	fmt.Printf("[Mangal]: New video uploaded title: %s \n", m.channel.(*striver).FetchMsg())
}

type shailu struct {
	channel Channel
	id      int
	name    string
}

func NewShailu(name string, ch *striver) *shailu {
	userId++
	return &shailu{
		id:      userId,
		name:    name,
		channel: ch,
	}
}
func (m *shailu) Id() int {
	return m.id
}
func (m *shailu) Update() {
	fmt.Printf("[Shailu]: New video uploaded title: %s \n", m.channel.(*striver).FetchMsg())
}
