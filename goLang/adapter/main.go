package main

import "adapter/adapter"

func main() { //client
	t := adapter.NewThirdParty()

	ad := adapter.NewAdapter(t)
	ad.C()

}
