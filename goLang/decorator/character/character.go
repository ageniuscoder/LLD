package character

import "fmt"

type Character interface {
	GetAbilities() string
}

type Mario struct {
}

func NewMario() *Mario {
	return &Mario{}
}

func (m *Mario) GetAbilities() string {
	return fmt.Sprintf("Hello %s", "Mario")
}
