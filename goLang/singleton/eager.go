package main

import "fmt"

type App struct {
	Name string
}

var instance *App = &App{      //eager initialization
	Name: "MyApp",
}

func getAppInstance() *App {
	return instance
}

func main() {
	a := getAppInstance()
    b:=getAppInstance()
	fmt.Println(a == b)
}