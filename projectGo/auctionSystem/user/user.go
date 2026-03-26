package user

import (
	"strconv"
	"sync/atomic"
)

var Id int64

type user struct {
	uid  string
	name string
}

func nextId() int64 {
	return atomic.AddInt64(&Id,1)
}

func NewUser(name string) *user {
	return &user{
		uid: "user"+strconv.FormatInt(nextId(),10),
		name: name,
	}
}

func (u *user) getId() string{
	return u.uid
}

func (u *user) GetName() string { return u.name }