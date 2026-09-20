package seat

import "fmt"

type Seat struct { // in row at what pos seat is
	seatid string
	row    int
	pos    int
}

func NewSeat(row, pos int) Seat {
	return Seat{
		seatid: fmt.Sprintf("s-%d%d", row, pos),
		row:    row,
		pos:    pos,
	}
}

func (s Seat) GetId() string {
	return s.seatid
}
