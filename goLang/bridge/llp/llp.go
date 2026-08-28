package llp

type Engine interface {
	Start() string
}

type Electric struct {
}

func NewElectric() Electric {
	return Electric{}
}

func (e Electric) Start() string {
	return "electric engine started"
}

type Petrol struct {
}

func NewPetrol() Petrol {
	return Petrol{}
}

func (e Petrol) Start() string {
	return "Petrol Engine started"
}
