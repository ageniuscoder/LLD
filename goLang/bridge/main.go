package main

import (
	"bridge/hlp"
	"bridge/llp"
)

func main() {
	electric := llp.NewElectric()
	petrol := llp.NewPetrol()

	suvwithpetrol := hlp.NewSuv(petrol)
	suvwithpetrol.Drive()

	sedanwithelectric := hlp.NewSedan(electric)
	sedanwithelectric.Drive()
}
