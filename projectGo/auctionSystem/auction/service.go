package auction

import (
	"auctionapp/bid"
	"auctionapp/enum"
	"auctionapp/notification"
	"fmt"
	"strconv"
	"sync/atomic"
	"time"
)

// ── ID generation (mirrors the pattern used in bid / bidItem / user) ─────────

var auctionCounter int64

func nextAuctionId() string {
	return "auction" + strconv.FormatInt(atomic.AddInt64(&auctionCounter, 1), 10)
}

// ── AuctionService ────────────────────────────────────────────────────────────

type AuctionService struct {
	auctions map[string]*Auction // auctionId -> *Auction
}

func NewAuctionService() *AuctionService {
	return &AuctionService{
		auctions: make(map[string]*Auction),
	}
}

// CreateAuction creates a new auction for a given BidItem and duration.
// The item owner is automatically registered as an observer (auctioneer).
// Returns the generated auction ID.
func (as *AuctionService) CreateAuction(item *bid.BidItem, dur time.Duration) (string, error) {
	if item == nil {
		return "", fmt.Errorf("bid item cannot be nil")
	}
	if dur <= 0 {
		return "", fmt.Errorf("auction duration must be positive")
	}

	auc := NewAuction(item, dur)
	auctionId := nextAuctionId()
	as.auctions[auctionId] = auc

	fmt.Printf("[AuctionService]: Auction %s created for item %s | base price: %d\n",
		auctionId, item.GetId(), item.GetBasePrice())

	return auctionId, nil
}

// StartAuction transitions an auction from PENDING → ACTIVE.
// It also schedules an automatic close after the configured duration
// using a background goroutine.
func (as *AuctionService) StartAuction(auctionId string) error {
	auc, err := as.getAuction(auctionId)
	if err != nil {
		return err
	}
	if auc.status != enum.PENDING {
		return fmt.Errorf("auction %s must be PENDING to start (current: %v)", auctionId, auc.status)
	}

	auc.UpdateStatus(enum.ACTIVE)
	fmt.Printf("[AuctionService]: Auction %s is now ACTIVE (duration: %s)\n", auctionId, auc.duration)

	// Auto-close after duration expires
	time.AfterFunc(auc.duration,func() {
		if closeErr := as.CloseAuction(auctionId); closeErr != nil {
			fmt.Printf("[AuctionService]: Error auto-closing auction %s: %v\n", auctionId, closeErr)
		}
	})

	return nil
}

// PlaceBid lets a user place a bid on an ACTIVE auction.
// Delegates validation and observer notification to Auction.DoBid.
func (as *AuctionService) PlaceBid(auctionId string, userId string, price int) error {
	auc, err := as.getAuction(auctionId)
	if err != nil {
		return err
	}
	if auc.status != enum.ACTIVE {
		return fmt.Errorf("auction %s is not active — bids cannot be placed", auctionId)
	}
	if userId == "" {
		return fmt.Errorf("user ID cannot be empty")
	}
	if price <= 0 {
		return fmt.Errorf("bid price must be a positive amount")
	}

	return auc.DoBid(userId, price)
}

// CloseAuction transitions an auction to CLOSED and declares a winner.
// The winner is the owner of the highest bid. If no bids were placed,
// the auction closes with no winner.
func (as *AuctionService) CloseAuction(auctionId string) error {
	auc, err := as.getAuction(auctionId)
	if err != nil {
		return err
	}
	if auc.status == enum.CLOSED {
		return fmt.Errorf("auction %s is already closed", auctionId)
	}

	auc.UpdateStatus(enum.CLOSED)

	winner := resolveWinner(auc)
	if winner != "" {
		auc.DecleareWinner(winner)
		fmt.Printf("[AuctionService]: Auction %s CLOSED | Winner: %s | Winning bid: %d\n",
			auctionId, winner, auc.maxBid)
	} else {
		fmt.Printf("[AuctionService]: Auction %s CLOSED | No bids were placed\n", auctionId)
	}

	return nil
}

// AddNotifier registers a Notifier (InApp, Email …) on an auction.
// Follows the Observer pattern — every new bid triggers all registered notifiers.
func (as *AuctionService) AddNotifier(auctionId string, n notification.Notifier) error {
	auc, err := as.getAuction(auctionId)
	if err != nil {
		return err
	}
	auc.AddNotifier(n)
	return nil
}

// AddNotifiers is a variadic convenience wrapper around AddNotifier.
func (as *AuctionService) AddNotifiers(auctionId string, notifiers ...notification.Notifier) error {
	for _, n := range notifiers {
		if err := as.AddNotifier(auctionId, n); err != nil {
			return err
		}
	}
	return nil
}

// GetAuctionStatus returns the current AuctionStatus for the given ID.
func (as *AuctionService) GetAuctionStatus(auctionId string) (enum.AuctionStatus, error) {
	auc, err := as.getAuction(auctionId)
	if err != nil {
		return 0, err
	}
	return auc.status, nil
}

// GetCurrentHighestBid returns the highest bid price seen so far.
func (as *AuctionService) GetCurrentHighestBid(auctionId string) (int, error) {
	auc, err := as.getAuction(auctionId)
	if err != nil {
		return 0, err
	}
	return auc.maxBid, nil
}

// GetWinner returns the winning user ID. Only valid after the auction is CLOSED.
func (as *AuctionService) GetWinner(auctionId string) (string, error) {
	auc, err := as.getAuction(auctionId)
	if err != nil {
		return "", err
	}
	if auc.status != enum.CLOSED {
		return "", fmt.Errorf("auction %s is not closed yet", auctionId)
	}
	if auc.winner == "" {
		return "", fmt.Errorf("no winner for auction %s — no bids were placed", auctionId)
	}
	return auc.winner, nil
}

// GetBidHistory returns the full ordered list of bids placed on an auction.
func (as *AuctionService) GetBidHistory(auctionId string) ([]*bid.Bid, error) {
	auc, err := as.getAuction(auctionId)
	if err != nil {
		return nil, err
	}
	return auc.bids, nil
}

// ── private helpers ───────────────────────────────────────────────────────────

func (as *AuctionService) getAuction(auctionId string) (*Auction, error) {
	auc, ok := as.auctions[auctionId]
	if !ok {
		return nil, fmt.Errorf("auction %s not found", auctionId)
	}
	return auc, nil
}

// resolveWinner picks the last bid in the slice — because DoBid only appends
// when price >= maxBid, the tail is always the winning bid.
func resolveWinner(auc *Auction) string {
	if len(auc.bids) == 0 {
		return ""
	}
	return auc.bids[len(auc.bids)-1].GetOwner()
}