package utility

import "tictactoe/enum"

type Board struct {
	size  int
	cells [][]*Cell
}

func NewBoard(size int) *Board {
	b := &Board{
		size:  size,
		cells: make([][]*Cell, size),
	}
	for i := range b.cells {
		b.cells[i] = make([]*Cell, size)
		for j := range b.cells[i] {
			b.cells[i][j] = NewCell(i, j)
		}
	}

	return b
}

func (b *Board) isValid(x, y int) bool {
	if x >= 0 && x < b.size && y >= 0 && y < b.size {
		return true
	}
	return false
}

func (b *Board) cellEmpty(x, y int) bool {
	if b.isValid(x, y) && b.cells[x][y].IsEmpty() {
		return true
	}
	return false
}

func (b *Board) PlaceSymbol(x, y int, symbol enum.Symbol) bool {
	if b.cellEmpty(x, y) {
		b.cells[x][y].Place(symbol)
		return true
	}
	return false
}

func (b *Board) IsFull() bool {
	for i := range b.cells {
		for j := range b.cells[i] {
			if b.cellEmpty(i, j) {
				return false
			}
		}
	}
	return true
}

func (b *Board) checkRow(x int, s enum.Symbol) bool {
	for _, c := range b.cells[x] {
		if !c.Contains(s) {
			return false
		}
	}
	return true
}

func (b *Board) checkColumn(y int, s enum.Symbol) bool {
	for i := 0; i < b.size; i++ {
		if !b.cells[i][y].Contains(s) {
			return false
		}
	}
	return true
}

func (b *Board) checkd1(s enum.Symbol) bool {
	for i := 0; i < b.size; i++ {
		if !b.cells[i][i].Contains(s) {
			return false
		}
	}
	return true
}

func (b *Board) checkd2(s enum.Symbol) bool {
	for i := 0; i < b.size; i++ {
		if !b.cells[i][b.size-i].Contains(s) {
			return false
		}
	}
	return false
}

func (b *Board) CheckWin(x, y int, symbol enum.Symbol) bool {
	if b.checkRow(x, symbol) {
		return true
	}

	if b.checkColumn(y, symbol) {
		return true
	}

	if x-y == 0 {
		if b.checkd1(symbol) {
			return true
		}
	}

	if x+y == b.size-1 {
		if b.checkd2(symbol) {
			return true
		}
	}

	return false
}
