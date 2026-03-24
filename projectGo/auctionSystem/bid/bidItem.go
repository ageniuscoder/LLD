package bid

import (
	"strconv"
	"sync/atomic"
)

var Id int64

func nextId() int64 {
	return atomic.AddInt64(&Id, 1)
}

type BidItem struct{
	id string
	owner string
	name string
	description string
	basePrice int
}

func NewBidItem(uid string,name string,des string,bp int) *BidItem{
	return &BidItem{
		id: "biditem"+strconv.FormatInt(nextId(),10),
		owner: uid,
		name: name,
		description: des,
		basePrice: bp,
	}
}

func (bi *BidItem) GetId() string{
	return bi.id
}

func (bi *BidItem) GetBasePrice() int{
	return bi.basePrice
}

func (bi *BidItem) GetOwner() string{
	return bi.owner
}