package obs

import "fmt"

type User interface {
	Update(string)
}
var userId int
type mangal struct {
	id   int
	name string
}

func NewMangal(name string) *mangal {
	userId++
	return &mangal{
		id: userId,
		name: name,
	}
}
func (m *mangal) Id() int{
	return m.id
}
func (m *mangal) Update(s string) {
	fmt.Printf("[Mangal]: New video uploaded title: %s \n",s)
}


type shailu struct {
	id   int
	name string
}

func NewShailu(name string) *shailu {
	userId++
	return &shailu{
		id: userId,
		name: name,
	}
}
func (m *shailu) Id() int{
	return m.id
}
func (m *shailu) Update(s string) {
	fmt.Printf("[Shailu]: New video uploaded title: %s \n",s)
}