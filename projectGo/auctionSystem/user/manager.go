package user

import "fmt"

type userManager struct {
	users map[string]*user
}

func NewUserManager() *userManager {
	return &userManager{
		users: make(map[string]*user),
	}
}

func (um *userManager) RegisterUser(name string) {
	user := NewUser(name)
	um.users[user.getId()] = user
	fmt.Printf("[uid: %s]: %s registered",user.getId(),name)
}
