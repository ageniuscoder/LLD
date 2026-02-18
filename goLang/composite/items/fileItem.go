package item

type FileSystemItem interface {
	Ls(indent int)
	OpenAll(indent int)
	GetSize() int
	GetName() string
}