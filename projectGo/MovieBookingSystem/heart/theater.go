package heart

type Theater struct {
	name    string
	screens []*Screen
}

func NewTheater(name string) *Theater {
	return &Theater{
		name:    name,
		screens: make([]*Screen, 0),
	}
}

func (t *Theater) GetName() string {
	return t.name
}

func (t *Theater) AddScreen(screen *Screen) {
	t.screens = append(t.screens, screen)
}
