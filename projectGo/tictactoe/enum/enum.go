package enum

type GameStatus int

const (
	_ GameStatus = iota
	WIN
	DRAW
	INGAME
)

type Symbol int

const (
	_ Symbol = iota
	E
	X
	O
)
