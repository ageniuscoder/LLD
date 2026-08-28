package main

import (
	"fmt"
	"proxy/resource"
)

func main() { //client
	res := resource.NewActualResource()
	proxy := resource.NewProxy(res)
	fmt.Println(proxy.Do())
}
