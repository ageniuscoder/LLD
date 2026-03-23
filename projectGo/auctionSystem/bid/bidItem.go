package bid

import (
	"strconv"
	"sync/atomic"
)

var Id int64

func nextId() int64 {
	return atomic.AddInt64(&Id, 1)
}

type bidItem struct{
	id string
	owner string
	name string
	description string
	basePrice int
}

func NewBidItem(uid string,name string,des string,bp int) *bidItem{
	return &bidItem{
		id: "biditem"+strconv.FormatInt(nextId(),10),
		owner: uid,
		name: name,
		description: des,
		basePrice: bp,
	}
}

func (bi *bidItem) GetId() string{
	return bi.id
}

func (bi *bidItem) GetBasePrice() int{
	return bi.basePrice
}