package auction

import (
	"auctionapp/bid"
	"auctionapp/enum"
	"auctionapp/notification"
	"fmt"
	"sync"
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
	mu sync.RWMutex
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
	au.mu.RLock()
	auct:=make([]string,len(au.auctioner))
	copy(auct,au.auctioner)
	noti:=make([]notification.Notifier,len(au.notifier))
	copy(noti,au.notifier)
	au.mu.RUnlock()
	for _,auct:=range auct{
		for _,a:=range noti{
			a.Update(auct,itemid,newPrice)
		}
	}
}


func (au *Auction) DoBid(uid string,price int) error{
	au.mu.Lock()
	if au.status != enum.ACTIVE {
		au.mu.Unlock() // Always unlock before returning
		return fmt.Errorf("auction is not active")
	}
	if price<=au.maxBid{
		au.mu.Unlock() // Always unlock before returning
		return fmt.Errorf("%s","Please Bid more Than current Bid price")
	}
	b:=bid.NewBid(uid,au.item.GetId(),price)
	au.maxBid=max(au.maxBid,price)
	au.bids=append(au.bids, b)
	au.AddAuctioner(uid)
	au.mu.Unlock()
	au.notify(au.item.GetId(),price)
	return nil
}

func (au *Auction) AddNotifier(n notification.Notifier){
	au.mu.Lock()
	defer au.mu.Unlock()
	au.notifier=append(au.notifier, n)
}

func (au *Auction) AddAuctioner(uid string){
	au.auctioner=append(au.auctioner, uid)
}

// FIX: Atomic state transition for Starting an auction.
func (au *Auction) Start() error {
	au.mu.Lock()
	defer au.mu.Unlock()
	if au.status != enum.PENDING {
		return fmt.Errorf("auction must be PENDING to start")
	}
	au.status = enum.ACTIVE
	return nil
}

// FIX: Atomic state transition for Closing and resolving the winner.
func (au *Auction) Close() (string, int, error) {
	au.mu.Lock()
	defer au.mu.Unlock()
	if au.status == enum.CLOSED {
		return "", 0, fmt.Errorf("auction is already closed")
	}
	au.status = enum.CLOSED

	if len(au.bids) > 0 {
		au.winner = au.bids[len(au.bids)-1].GetOwner()
	}
	return au.winner, au.maxBid, nil
}

func (au *Auction) GetStatus() enum.AuctionStatus {
	au.mu.RLock()
	defer au.mu.RUnlock()
	return au.status
}

func (au *Auction) GetMaxBid() int {
	au.mu.RLock()
	defer au.mu.RUnlock()
	return au.maxBid
}

func (au *Auction) GetWinner() string {
	au.mu.RLock()
	defer au.mu.RUnlock()
	return au.winner
}

func (au *Auction) GetBids() []*bid.Bid {
	au.mu.RLock()
	defer au.mu.RUnlock()
	
	// FIX: Returning the slice directly is a race condition because DoBid might 
	// append to it later. We return a safe copy of the current bids.
	bidsCopy := make([]*bid.Bid, len(au.bids))
	copy(bidsCopy, au.bids)
	return bidsCopy
}