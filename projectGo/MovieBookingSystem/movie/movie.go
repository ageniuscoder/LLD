package movie

import "time"

type Movie struct {
	name string
	dur  time.Duration
}

func NewMovie(name string, dur int) Movie { //duration in minutes
	return Movie{
		name: name,
		dur:  time.Duration(dur) * time.Minute,
	}
}

func (m Movie) GetName() string {
	return m.name
}

func (m Movie) GetDur() time.Duration {
	return m.dur
}
