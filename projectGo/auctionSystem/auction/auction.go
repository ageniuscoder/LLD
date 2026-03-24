package auction

import (
	"auctionapp/bid"
	"auctionapp/enum"
	"auctionapp/notification"
	"fmt"
	"time"
)

type Auction struct {
	item *bid.BidItem
	bids []*bid.Bid
	status enum.AuctionStatus
	duration time.Duration
	maxBid int
	winner string
	auctioner []string
	notifier []notification.Notifier
}

func NewAuction(item *bid.BidItem,dur time.Duration) *Auction{
	auc:=&Auction{
		item: item,
		duration: dur,
		status: enum.PENDING,
		maxBid: item.GetBasePrice(),
	}
	auc.auctioner=append(auc.auctioner, auc.item.GetOwner())
	return  auc
}

func (au *Auction) notify(itemid string, newPrice int){
	for _,auct:=range au.auctioner{
		for _,a:=range au.notifier{
			a.Update(auct,itemid,newPrice)
		}
	}
}


func (au *Auction) DoBid(uid string,price int) error{
	if price<au.maxBid{
		return fmt.Errorf("%s","Please Bid more Than current Bid price")
	}
	b:=bid.NewBid(uid,au.item.GetId(),price)
	au.maxBid=max(au.maxBid,price)
	au.bids=append(au.bids, b)
	au.AddAuctioner(uid)
	au.notify(au.item.GetId(),price)
	return nil
}
func (au *Auction) UpdateStatus(st enum.AuctionStatus){
	au.status=st
}

func (au *Auction) DecleareWinner(id string){
	au.winner=id
}

func (au *Auction) AddNotifier(n notification.Notifier){
	au.notifier=append(au.notifier, n)
}

func (au *Auction) AddAuctioner(uid string){
	au.auctioner=append(au.auctioner, uid)
}