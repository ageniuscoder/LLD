package main

import "fmt"

type Character interface {
	GetAbilities() string
}
type Shooter interface{
	Shoot()
}
type Mario struct{
	ammo int
}

func NewMario() *Mario{
	return &Mario{
		ammo: 0,
	}
}

func (m *Mario) GetAbilities() string {
	return fmt.Sprintf("Hello %s","Mario")
}


type GunAbility struct{
	c Character           //here character is interface so it store intrface type + pointer to the value so no need to make it pointer
	ammo int
}
func NewGunAbility(c Character,ammo int) *GunAbility{         //in decorator constructor always pass interface not concrete type
	return &GunAbility{
		c:c,
		ammo: ammo,
	}
}
func (g *GunAbility) Shoot(){
	if g.ammo>=5{
		g.ammo-=5;
		fmt.Printf("Ammo remaining: %d\n", g.ammo)
	}else{
		fmt.Println("not enough ammo to fire")
	}
}
func (g *GunAbility) GetAbilities() string{
	return  fmt.Sprintf("%s with gun ability (%d ammo)",
		g.c.GetAbilities(), g.ammo)
}
type JumpAbility struct{
	c Character
}
func NewJumpAbility(c Character) *JumpAbility{
	return &JumpAbility{
		c:c,
	}
}
func (j *JumpAbility) Shoot(){  //explicitly forwarding of gunAbility shoot method (Forwarding Optional Behavior)
	if shooter,ok:=j.c.(Shooter); ok{   //here type assertion check dynamic type,bcz interface stores (dynamic type,dynamic value),, for marioWithGunJump Dynamic type is actually *GunAbility so shoot works
		shooter.Shoot()
	}else{
		fmt.Println("can,t Shoot with only Jump power")
	}
}
func (j *JumpAbility) GetAbilities() string{
	return j.c.GetAbilities()+" with jump power"
}
func main() {   //first get gun ability then get jump ability
	mario:=NewMario()
	fmt.Println(mario.GetAbilities())

	marioWithGun:=NewGunAbility(mario,10)
	fmt.Println(marioWithGun.GetAbilities())
	marioWithGun.Shoot()

	marioWithJump:=NewJumpAbility(mario)
	fmt.Println(marioWithJump.GetAbilities())
	marioWithJump.Shoot()              //but here dynamic type is *Mario and it doesn,t implemnet shoot so can,t shoot here
	marioWithGunJump:=NewJumpAbility(marioWithGun)
	fmt.Println(marioWithGunJump.GetAbilities())
	marioWithGunJump.Shoot()
}

//Methods are available based on the static type of the variable.
//Even though inside it wraps a GunAbility, Go does not automatically "forward" methods.
//Go does not do dynamic method discovery like some OOP languages.