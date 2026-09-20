package heart

import (
	"bookmyshow/movie"
	"bookmyshow/seat"
	"fmt"
	"time"
)

var id int

func next() string {
	id += 1
	return fmt.Sprintf("screen-%d", id)
}

type Screen struct {
	id    string
	seats []seat.Seat
	shows map[time.Time]*Show
}

func NewScreen() *Screen {
	s := &Screen{
		id:    next(),
		shows: make(map[time.Time]*Show),
	}
	for i := range 5 {
		for j := range 6 {
			s.seats = append(s.seats, seat.NewSeat(i+1, j+1))
		}
	}
	return s
}

func (s *Screen) AddShow(start time.Time, movie movie.Movie) {
	s.shows[start] = NewShow(s, movie, 4)
}

func (s *Screen) GetSeats() []seat.Seat {
	return s.seats
}
