package item

import (
	"fmt"
	"strings"
)

type file struct {
	name string
	size int
}

func NewFile(name string, size int) *file {
	return &file{
		name: name,
		size: size,
	}
}
func (f *file) GetName() string {
	return f.name
}
func (f *file) GetSize() int {
	return f.size
}
func (f *file) Ls(indent int) {
	fmt.Println(strings.Repeat("   ",indent)+f.name)
}
func (f *file) OpenAll(indent int){
	fmt.Println(strings.Repeat("   ",indent)+f.name)
}