package item

import (
	"fmt"
	"strings"
)

type folder struct {
	name  string
	items []FileSystemItem
}

func NewFolder(name string) *folder {
	return &folder{
		name: name,
	}
}
func (f *folder) Add(i FileSystemItem) {
	f.items = append(f.items, i)
}

func (f *folder) GetName() string {
	return fmt.Sprintf("+%s",f.name)
}
func (f *folder) GetSize() int {
	size := 0
	for _, i := range f.items {
		size += i.GetSize()
	}
	return size
}

func (f *folder) Ls(indent int) {
	fmt.Println(strings.Repeat("   ",indent)+f.GetName())
	for _,i:=range f.items{
		fmt.Println(strings.Repeat("   ",indent+1)+i.GetName())
	}
}

func (f *folder) OpenAll(indent int){
	fmt.Println(strings.Repeat("   ",indent)+f.GetName())
	for _,i:=range f.items{
		i.OpenAll(indent+1)
	}
}