package main

import "cor/handler"

func main() {
	khandler := handler.NewThousandHandler(3)
	thandler := handler.NewTwoHundredHandler(5)
	ohandler := handler.NewHundredHandler(3)

	khandler.SetNext(thandler)
	thandler.SetNext(ohandler)

	khandler.Dispense(2360)
}
