package game

import (
	"fmt"
	"tictactoe/enum"
	"tictactoe/player"
	"tictactoe/utility"
)

type Game struct {
	board  *utility.Board
	px     *player.Player
	po     *player.Player
	status enum.GameStatus
	turn   enum.Symbol
}

func NewGame(board *utility.Board, px, po *player.Player) *Game {
	g := &Game{
		board:  board,
		px:     px,
		po:     po,
		status: enum.INGAME,
		turn:   enum.X,
	}
	g.px.SetSymbol(enum.X)
	g.po.SetSymbol(enum.O)
	return g
}

func (g *Game) changeTurn() {
	if g.turn == enum.X {
		g.turn = enum.O
	} else {
		g.turn = enum.X
	}
}

func (g *Game) makeMove(x, y int) bool {
	if g.turn == enum.X {
		return g.board.PlaceSymbol(x, y, g.px.GetSymbol())
	}
	return g.board.PlaceSymbol(x, y, g.po.GetSymbol())
}

func (g *Game) play() {

	name := ""
	if g.turn == enum.X {
		name = g.px.GetName()
	} else {
		name = g.po.GetName()
	}
	var row, col int

	fmt.Printf("%s, make move (enter row and column): ", name)
	fmt.Scan(&row, &col)

	if !g.makeMove(row, col) {
		fmt.Println("Invalid Move, Try again")
		return
	}

	// Check whether current player won
	if g.board.CheckWin(row, col, g.turn) {
		if g.turn == enum.X {
			g.status = enum.WINNER_X
		} else {
			g.status = enum.WINNER_O
		}
		return
	}

	// Check draw
	if g.board.IsFull() {
		g.status = enum.DRAW
		return
	}

	// Switch player
	g.changeTurn()
}

func (g *Game) Start() {
	for g.status == enum.INGAME {
		g.board.Print()
		g.play()
	}

	g.board.Print()

	fmt.Println("Game Over:", g.status.String())
}
