package bid

import (
	"strconv"
	"sync/atomic"
)

var BidId int64

func nextBidId() int64 {
	return atomic.AddInt64(&BidId, 1)
}

type bid struct{
	id string
	owner string
	itemId string
	price int
}

func NewBid(uid string,itemId string,price int) *bid{
	return &bid{
		id: "bid"+strconv.FormatInt(nextBidId(),10),
		owner: uid,
		itemId: itemId,
		price: price,
	}
}

func (b *bid) GetId() string{
	return b.id
}

func (b *bid) GetOwner() string{
	return b.owner
}

func (b *bid) GetBidItem() string{
	return b.itemId
}

func (b *bid) GetPrice() int{
	return b.price
}
