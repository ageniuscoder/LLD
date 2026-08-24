package main //code base with explicit forwarding

import (
	"decorator/character"
	deco "decorator/decorator"
	"fmt"
)

func main() { //first get gun ability then get jump ability
	mario := character.NewMario()
	fmt.Println(mario.GetAbilities())

	marioWithGun := deco.NewGunAbility(mario)
	fmt.Println(marioWithGun.GetAbilities())

	marioWithJump := deco.NewJumpAbility(mario)
	fmt.Println(marioWithJump.GetAbilities())
	//marioWithJump.Shoot() //but here dynamic type is *Mario and it doesn,t implemnet shoot so can,t shoot here
	marioWithGunJump := deco.NewJumpAbility(marioWithGun)
	fmt.Println(marioWithGunJump.GetAbilities())
}

//Methods are available based on the static type of the variable.
//Even though inside it wraps a GunAbility, Go does not automatically "forward" methods.
//Go does not do dynamic method discovery like some OOP languages.
