package song

import (
	"itert/iterator"
	"time"
)

type song struct {
	name     string
	duration time.Duration
}

func NewSong(name string, d time.Duration) song {
	return song{
		name:     name,
		duration: d,
	}
}

type playList struct {
	name  string
	songs []song
}

func NewPlaylist(name string) *playList {
	return &playList{
		name: name,
	}
}

func (p *playList) AddSong(s song) {
	p.songs = append(p.songs, s)
}

func (p *playList) GetIterator() iterator.Iterator[song] {
	return NewPlayListIter(p.songs)
}

type PlayListIter struct {
	songs   []song
	curridx int
}

func NewPlayListIter(songs []song) *PlayListIter {
	return &PlayListIter{
		songs:   songs,
		curridx: 0,
	}
}

func (pll *PlayListIter) IsNext() bool {
	n := len(pll.songs)
	return pll.curridx != n
}

func (pll *PlayListIter) Next() song {
	cs := pll.songs[pll.curridx]
	pll.curridx++
	return cs
}
