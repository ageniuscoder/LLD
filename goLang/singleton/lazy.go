package main

import (
	"fmt"
	"sync"
)

type Config struct {
	Name string
}

var (
	instance *Config
	once     sync.Once
)

func getInstance() *Config{       //lazy initialization
	once.Do(func(){
		instance=&Config{
			Name: "MyApp",
		}
	})
	return instance
}

func main(){
	a:=getInstance()
	b:=getInstance()
	fmt.Println(a==b)
}
