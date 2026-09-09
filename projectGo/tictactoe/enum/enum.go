package enum

type GameStatus int

const (
	_ GameStatus = iota
	WINNER_X
	WINNER_O
	DRAW
	INGAME
)

func (g GameStatus) String() string {
	switch g {
	case WINNER_X:
		return "WINNER_X"
	case WINNER_O:
		return "WINNER_O"
	case DRAW:
		return "DRAW"
	case INGAME:
		return "INGAME"
	default:
		return "UNKNOWN"
	}
}

type Symbol int

const (
	_ Symbol = iota
	E
	X
	O
)

func (s Symbol) String() string {
	switch s {
	case E:
		return "E"
	case X:
		return "X"
	case O:
		return "O"
	default:
		return "?"
	}
}
