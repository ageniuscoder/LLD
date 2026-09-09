package main

import (
	"tictactoe/game"
	"tictactoe/player"
	"tictactoe/utility"
)

func main() {
	board := utility.NewBoard(3)
	mangal := player.NewPlayer("mangal")
	manya := player.NewPlayer("manya")

	game := game.NewGame(board, mangal, manya)

	game.Start()
}
