package deco

import (
	"decorator/character"
	"fmt"
)

type Decorator struct {
	c character.Character
}

type GunAbility struct {
	Decorator
}

func NewGunAbility(c character.Character) *GunAbility { //in decorator constructor always pass interface not concrete type
	return &GunAbility{
		Decorator: Decorator{c},
	}
}

func (g *GunAbility) GetAbilities() string {
	return fmt.Sprintf("%s with gun ability",
		g.c.GetAbilities())
}

type JumpAbility struct {
	Decorator
}

func NewJumpAbility(c character.Character) *JumpAbility {
	return &JumpAbility{
		Decorator: Decorator{c},
	}
}

func (j *JumpAbility) GetAbilities() string {
	return j.c.GetAbilities() + " with jump power"
}
