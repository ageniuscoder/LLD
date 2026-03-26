package main

import (
	"auctionapp/auction"
	"auctionapp/notification"
	"auctionapp/user"
	"fmt"
	"time"
)

func main() {
	fmt.Println("==============================================")
	fmt.Println("          Auction App — Demo Run             ")
	fmt.Println("==============================================")

	// ── 1. Dependency Injection ───────────────────────────────────────────────
	// Each subsystem is constructed independently and injected into the Facade.
	// Swapping either for a mock in tests requires zero changes to the Facade.
	um := user.NewUserManager()
	as := auction.NewAuctionService()
	app := NewAuctionApp(um, as)

	// ── 2. Register users ─────────────────────────────────────────────────────
	fmt.Println("\n--- Registering Users ---")
	sellerUid  := app.RegisterUser("Alice")    // item owner / seller
	bobUid     := app.RegisterUser("Bob")      // bidder
	charlieUid := app.RegisterUser("Charlie")  // bidder
	dianaUid   := app.RegisterUser("Diana")    // bidder

	// ── 3. Create auction (Facade hides BidItem construction) ─────────────────
	fmt.Println("\n--- Creating Auction ---")
	auctionId, err := app.CreateAuction(
		sellerUid,
		"Vintage Guitar",
		"1962 Fender Stratocaster — mint condition",
		500,             // base price
		10*time.Second,  // auto-closes after 10 s
		notification.NewInAppNotification(), // Observer / Strategy — channel 1
		notification.NewEmailNotification(), // Observer / Strategy — channel 2
	)
	mustOk(err)
	fmt.Printf("Auction created: %s\n", auctionId)

	// ── 4. Start ──────────────────────────────────────────────────────────────
	fmt.Println("\n--- Starting Auction ---")
	mustOk(app.StartAuction(auctionId))

	// ── 5. Place bids ─────────────────────────────────────────────────────────
	fmt.Println("\n--- Placing Bids ---")
	placeBid(app, auctionId, bobUid,     550) // ✓ above base price
	placeBid(app, auctionId, charlieUid, 520) // ✗ below current max
	placeBid(app, auctionId, charlieUid, 600) // ✓
	placeBid(app, auctionId, dianaUid,   600) // ✗ not strictly greater
	placeBid(app, auctionId, dianaUid,   750) // ✓ Diana leads
	placeBid(app, auctionId, bobUid,     800) // ✓ Bob reclaims lead



	time.Sleep(15*time.Second)

	// ── 6. Live state ─────────────────────────────────────────────────────────
	fmt.Println("\n--- Current Highest Bid ---")
	highest, err := app.GetHighestBid(auctionId)
	mustOk(err)
	fmt.Printf("Highest bid: %d\n", highest)

	// ── 8. Results ────────────────────────────────────────────────────────────
	fmt.Println("\n--- Bid History ---")
	mustOk(app.PrintBidHistory(auctionId))

	fmt.Println("\n--- Winner ---")
	winner, err := app.GetWinner(auctionId)
	mustOk(err)
	fmt.Printf("Winner: %s\n", winner)

	// ── 9. Edge-case guard: bid after close ───────────────────────────────────
	fmt.Println("\n--- Bid after close (should fail) ---")
	placeBid(app, auctionId, charlieUid, 999)

	// ── 10. Edge-case guard: unregistered user bids ───────────────────────────
	fmt.Println("\n--- Unregistered user bids (should fail) ---")
	placeBid(app, auctionId, "ghost-uid", 1000)

	fmt.Println("\n==============================================")
	fmt.Println("                   Done                      ")
	fmt.Println("==============================================")
}

func placeBid(app *AuctionApp, auctionId, uid string, price int) {
	if err := app.PlaceBid(auctionId, uid, price); err != nil {
		fmt.Printf("  ✗ bid uid=%-8s price=%-5d → %v\n", uid, price, err)
	} else {
		fmt.Printf("  ✓ bid uid=%-8s price=%d\n", uid, price)
	}
}

func mustOk(err error) {
	if err != nil {
		panic(err)
	}
}