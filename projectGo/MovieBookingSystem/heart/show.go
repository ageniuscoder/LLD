package heart

import (
	"bookmyshow/movie"
	"time"
)

type Show struct {
	movie movie.Movie
	seats *ShowSeat
}

func NewShow(screen *Screen, movie movie.Movie, timeout int) *Show {
	return &Show{
		seats: NewShowSeat(screen, time.Duration(timeout)*time.Second),
		movie: movie,
	}
}

func (s *Show) GetDuration() time.Duration {
	return s.movie.GetDur()
}

func (s *Show) GetShowSeats() *ShowSeat {
	return s.seats
}
