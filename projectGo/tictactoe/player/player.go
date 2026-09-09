package player

import "tictactoe/enum"

var counter int

func next() int {
	counter++
	return counter
}

type Player struct {
	id     int
	name   string
	symbol enum.Symbol
}

func NewPlayer(name string) *Player {
	return &Player{
		id:     next(),
		name:   name,
		symbol: enum.E,
	}
}

func (p *Player) GetId() int {
	return p.id
}

func (p *Player) GetName() string {
	return p.name
}

func (p *Player) GetSymbol() enum.Symbol {
	return p.symbol
}

func (p *Player) SetSymbol(s enum.Symbol) {
	p.symbol = s
}
