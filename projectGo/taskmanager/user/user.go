package user

import (
	"fmt"
	"strconv"
	"sync"
	"sync/atomic"
)

var Id int64

type User struct {
	uid   string
	uname string
	email string
}

func nextId() int64{
	id:=atomic.AddInt64(&Id,1)
	return id
}

func NewUser(uname string, email string) *User {
	return &User{
		uid: "user" + strconv.FormatInt(nextId(),10),
		uname: uname,
		email: email,
	}
}

func (u *User) GetId() string{
	return u.uid
}


type UserManager struct{
	users map[string]*User
	rwm sync.RWMutex
}

func NewUserManager() *UserManager{
	return &UserManager{
		users: make(map[string]*User),
	}
}

func (um *UserManager) AddUser(user *User){
	um.rwm.Lock()
	defer um.rwm.Unlock()
	uid:=user.GetId()
	if _,ok:=um.users[uid]; ok{
		fmt.Printf("User already exists with uid %s",uid)
	}
	um.users[uid]=user
}