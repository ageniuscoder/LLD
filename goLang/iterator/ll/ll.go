package ll

import (
	"itert/iterator"
)

type node[T any] struct {
	data T
	next *node[T]
}

func newNode[T any](data T) *node[T] {
	return &node[T]{
		data: data,
		next: nil,
	}
}

type Ll[T any] struct {
	head *node[T]
}

func NewLl[T any]() *Ll[T] {
	return &Ll[T]{
		head: nil,
	}
}

func (ll *Ll[T]) InsertEnd(data T) {
	node := newNode(data)

	if ll.head == nil {
		ll.head = node
		return
	}

	temp := ll.head

	for temp.next != nil {
		temp = temp.next
	}

	temp.next = node
}

func (ll *Ll[T]) GetIterator() iterator.Iterator[T] {
	return NewLlIterator(ll.head)
}

type LlIterator[T any] struct {
	curr *node[T]
}

func NewLlIterator[T any](node *node[T]) *LlIterator[T] {
	return &LlIterator[T]{
		curr: node,
	}
}

func (lli *LlIterator[T]) IsNext() bool {
	return lli.curr != nil
}

func (lli *LlIterator[T]) Next() T {
	data := lli.curr.data
	lli.curr = lli.curr.next
	return data
}
