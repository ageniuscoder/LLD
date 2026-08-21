package iterable

import "itert/iterator"

type Iterable[T any] interface {
	GetIterator() *iterator.Iterator[T]
}
