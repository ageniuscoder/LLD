package utility

import "tictactoe/enum"

type Cell struct {
	x      int
	y      int
	symbol enum.Symbol
}

func NewCell(x, y int) *Cell {
	return &Cell{
		x:      x,
		y:      y,
		symbol: enum.E,
	}
}

func (c *Cell) GetPos() (int, int) {
	return c.x, c.y
}

func (c *Cell) IsEmpty() bool {
	if c.symbol == enum.E {
		return true
	}
	return false
}

func (c *Cell) Place(symbol enum.Symbol) {
	c.symbol = symbol
}

func (c *Cell) Contains(s enum.Symbol) bool {
	if c.symbol == s {
		return true
	}
	return false
}
