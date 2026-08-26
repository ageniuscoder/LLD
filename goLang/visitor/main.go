package main

import "visitor/visitor"

func main() {
	dtext := visitor.NewText(5, "hello")
	dimage := visitor.NewImage(10)

	dtext.Accept(visitor.NewCalcSize())
	dtext.Accept(visitor.NewCompress())
	dimage.Accept(visitor.NewCompress())
	dimage.Accept(visitor.NewCalcSize())
}
