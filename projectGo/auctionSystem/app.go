package main

import (
	"auctionapp/auction"
	"auctionapp/bid"
	"auctionapp/notification"
	"auctionapp/user"
	"fmt"
	"time"
)

// AuctionApp is a Facade that unifies UserManager and AuctionService under
// a single, use-case-driven API. Both dependencies are injected via the
// constructor, making the facade easy to test and extend.
type AuctionApp struct {
	userManager    *user.UserManager
	auctionService *auction.AuctionService
}

// NewAuctionApp constructs the Facade with its two dependencies injected.
func NewAuctionApp(um *user.UserManager, as *auction.AuctionService) *AuctionApp {
	return &AuctionApp{
		userManager:    um,
		auctionService: as,
	}
}

// ── User operations ───────────────────────────────────────────────────────────

// RegisterUser registers a new user and returns their uid.
func (app *AuctionApp) RegisterUser(name string) string {
	return app.userManager.RegisterUser(name)
}

// ── Auction lifecycle ─────────────────────────────────────────────────────────

// CreateAuction builds a BidItem owned by ownerUid, creates an Auction for it,
// attaches the provided Notifiers (Observer pattern), and returns the auctionId.
func (app *AuctionApp) CreateAuction(
	ownerUid string,
	itemName string,
	itemDescription string,
	basePrice int,
	duration time.Duration,
	notifiers ...notification.Notifier,
) (string, error) {
	if !app.userManager.Exists(ownerUid) {
		return "", fmt.Errorf("[AuctionApp]: user %s is not registered", ownerUid)
	}
	item := bid.NewBidItem(ownerUid, itemName, itemDescription, basePrice)
	auctionId, err := app.auctionService.CreateAuction(item, duration)
	if err != nil {
		return "", fmt.Errorf("[AuctionApp]: failed to create auction: %w", err)
	}
	if err := app.auctionService.AddNotifiers(auctionId, notifiers...); err != nil {
		return "", fmt.Errorf("[AuctionApp]: failed to add notifiers: %w", err)
	}
	return auctionId, nil
}

// StartAuction transitions the auction from PENDING → ACTIVE.
func (app *AuctionApp) StartAuction(auctionId string) error {
	return app.auctionService.StartAuction(auctionId)
}

// PlaceBid validates the user exists then delegates to AuctionService.
func (app *AuctionApp) PlaceBid(auctionId, uid string, price int) error {
	if !app.userManager.Exists(uid) {
		return fmt.Errorf("[AuctionApp]: user %s is not registered", uid)
	}
	return app.auctionService.PlaceBid(auctionId, uid, price)
}


// ── Query helpers ─────────────────────────────────────────────────────────────

// GetWinner returns a human-readable winner string ("Name (uid: xxx)").
func (app *AuctionApp) GetWinner(auctionId string) (string, error) {
	uid, err := app.auctionService.GetWinner(auctionId)
	if err != nil {
		return "", err
	}
	if u := app.userManager.GetUser(uid); u != nil {
		return fmt.Sprintf("%s (uid: %s)", u.GetName(), uid), nil
	}
	return uid, nil
}

// GetHighestBid returns the current highest bid price.
func (app *AuctionApp) GetHighestBid(auctionId string) (int, error) {
	return app.auctionService.GetCurrentHighestBid(auctionId)
}

// PrintBidHistory prints the full ordered bid history to stdout.
func (app *AuctionApp) PrintBidHistory(auctionId string) error {
	bids, err := app.auctionService.GetBidHistory(auctionId)
	if err != nil {
		return err
	}
	if len(bids) == 0 {
		fmt.Printf("[AuctionApp]: No bids placed on %s\n", auctionId)
		return nil
	}
	fmt.Printf("[AuctionApp]: Bid history for %s:\n", auctionId)
	for i, b := range bids {
		name := b.GetOwner()
		if u := app.userManager.GetUser(b.GetOwner()); u != nil {
			name = u.GetName()
		}
		fmt.Printf("  #%d  bidder=%-10s  price=%d\n", i+1, name, b.GetPrice())
	}
	return nil
}