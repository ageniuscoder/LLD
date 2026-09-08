package player

var counter int

func next() int {
	counter++
	return counter
}

type Player struct {
	id   int
	name string
}

func NewPlayer(name string) Player {
	return Player{
		id:   next(),
		name: name,
	}
}

func (p Player) GetId() int {
	return p.id
}

func (p Player) GetName() string {
	return p.name
}
