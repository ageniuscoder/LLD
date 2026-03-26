package main

import (
	"auctionapp/auction"
	"auctionapp/user"
	"fmt"
	"math/rand"
	"sync"
	"sync/atomic"
	"time"
)

func main() {
	fmt.Println("==============================================")
	fmt.Println("   Auction App — Concurrency Stress Test      ")
	fmt.Println("==============================================")

	// ── 1. Setup App ──────────────────────────────────────────────────────────
	um := user.NewUserManager()
	as := auction.NewAuctionService()
	app := NewAuctionApp(um, as)

	// ── 2. Register users ─────────────────────────────────────────────────────
	sellerUid := app.RegisterUser("Alice")
	bidders := []string{
		app.RegisterUser("Bob"),
		app.RegisterUser("Charlie"),
		app.RegisterUser("Diana"),
		app.RegisterUser("Eve"),
		app.RegisterUser("Frank"),
	}

	// ── 3. Create & Start Auction ─────────────────────────────────────────────
	// We omit notifiers here to prevent the console from being flooded with 
	// thousands of print statements during the load test.
	auctionId, err := app.CreateAuction(
		sellerUid,
		"Rolex Watch",
		"Submariner Date",
		1000,           // Base Price
		3*time.Second,  // Closes very fast (3 seconds) to test TOCTOU race condition
	)
	mustOk(err)

	fmt.Println("\n--- Starting Auction ---")
	mustOk(app.StartAuction(auctionId))

	// ── 4. Concurrency Test Setup ─────────────────────────────────────────────
	var wg sync.WaitGroup
	var successfulBids int32 // Use atomic counter to track how many bids actually went through
	var failedBids int32

	totalConcurrentBids := 1000 // 1000 simultaneous bid attempts

	fmt.Printf("\n--- Launching %d Concurrent Bids & Readers ---\n", totalConcurrentBids)

	// ── 5. Spawn Concurrent Bidders (Writers) ─────────────────────────────────
	for i := 0; i < totalConcurrentBids; i++ {
		wg.Add(1)
		go func(bidderId string) {
			defer wg.Done()
			
			// Generate a random bid price between 1000 and 5000
			randomPrice := 1000 + rand.Intn(4000)

			err := app.PlaceBid(auctionId, bidderId, randomPrice)
			if err == nil {
				atomic.AddInt32(&successfulBids, 1)
			} else {
				atomic.AddInt32(&failedBids, 1)
			}
		}(bidders[i%len(bidders)]) // Round-robin through available bidders
	}

	// ── 6. Spawn Concurrent Readers ───────────────────────────────────────────
	// While bids are happening, constantly try to read the highest bid and history
	// to ensure read/write locks are protecting the state without deadlocking.
	for i := 0; i < 5; i++ {
		wg.Add(1)
		go func() {
			defer wg.Done()
			// Rapidly check state 10 times per goroutine
			for j := 0; j < 10; j++ {
				_, _ = app.GetHighestBid(auctionId)
				// We intentionally ignore the error here to not clutter the console
				// but running this method heavily tests the slice read locks.
				_ = app.PrintBidHistory(auctionId) 
				time.Sleep(5 * time.Millisecond)
			}
		}()
	}

	// ── 7. Wait for all operations to finish ──────────────────────────────────
	wg.Wait()
	fmt.Println("\n--- All Concurrent Operations Finished ---")
	
	fmt.Printf("Successful Bids: %d\n", successfulBids)
	fmt.Printf("Failed/Rejected Bids: %d (due to being lower than max, or auction closed)\n", failedBids)

	// Wait to ensure the auto-close timer fires
	fmt.Println("\nWaiting for auction timer to automatically close...")
	time.Sleep(4 * time.Second)

	// ── 8. Final Results ──────────────────────────────────────────────────────
	fmt.Println("\n==============================================")
	fmt.Println("                  RESULTS                     ")
	fmt.Println("==============================================")

	highest, err := app.GetHighestBid(auctionId)
	mustOk(err)
	fmt.Printf("Final Highest Bid: %d\n", highest)

	winner, err := app.GetWinner(auctionId)
	if err != nil {
		fmt.Printf("Winner Error: %v\n", err)
	} else {
		fmt.Printf("Winner: %s\n", winner)
	}
	
	fmt.Println("==============================================")
}

func mustOk(err error) {
	if err != nil {
		panic(err)
	}
}