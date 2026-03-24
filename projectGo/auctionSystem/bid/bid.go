package bid

import (
	"strconv"
	"sync/atomic"
)

var BidId int64

func nextBidId() int64 {
	return atomic.AddInt64(&BidId, 1)
}

type Bid struct{
	id string
	owner string
	itemId string
	price int
}

func NewBid(uid string,itemId string,price int) *Bid{
	return &Bid{
		id: "bid"+strconv.FormatInt(nextBidId(),10),
		owner: uid,
		itemId: itemId,
		price: price,
	}
}

func (b *Bid) GetId() string{
	return b.id
}

func (b *Bid) GetOwner() string{
	return b.owner
}

func (b *Bid) GetBidItem() string{
	return b.itemId
}

func (b *Bid) GetPrice() int{
	return b.price
}
