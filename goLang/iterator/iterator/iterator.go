package iterator

type Iterator[T any] interface {
	IsNext() bool
	Next() T
}
