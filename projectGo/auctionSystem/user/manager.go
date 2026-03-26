package user

import (
	"fmt"
	"sync"
)

// UserManager is exported so main package can reference it in DI.
type UserManager struct {
	users map[string]*user
	mu sync.RWMutex
}

func NewUserManager() *UserManager {
	return &UserManager{users: make(map[string]*user)}
}

// RegisterUser creates a user and returns the generated uid.
func (um *UserManager) RegisterUser(name string) string {
	um.mu.Lock()
	defer um.mu.Unlock()
	u := NewUser(name)
	um.users[u.getId()] = u
	fmt.Printf("[UserManager]: uid=%-8s name=%s registered\n", u.getId(), name)
	return u.getId()
}

// GetUser returns the user for uid, or nil if not found.
func (um *UserManager) GetUser(uid string) *user {
	um.mu.RLock()
	defer um.mu.RUnlock()
	return um.users[uid]
}

// Exists reports whether uid is registered.
func (um *UserManager) Exists(uid string) bool {
	um.mu.RLock()
	defer um.mu.RUnlock()
	_, ok := um.users[uid]
	return ok
}