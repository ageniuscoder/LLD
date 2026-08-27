package main

import "state/state"

func main() {
	ctx := state.NewContext()
	ctx.M4()
	ctx.M1()
	ctx.M1()
	ctx.M2()
	ctx.M3()
	ctx.M4()
	ctx.M4()
}
