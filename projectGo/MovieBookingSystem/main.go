package main

import (
	"bookmyshow/app"
	"bookmyshow/bookingservice"
	"bookmyshow/heart"
	"bookmyshow/movie"
	"fmt"
	"sync"
	"time"
)

func main() {
	fmt.Println("========================================")
	fmt.Println("       BOOK MY SHOW - CONCURRENCY TEST")
	fmt.Println("========================================")

	// --------------------------------------------------
	// Setup
	// --------------------------------------------------

	theater := heart.NewTheater("PVR Cinemas")

	screen := heart.NewScreen()
	theater.AddScreen(screen)

	movie := movie.NewMovie("Interstellar", 180)

	startTime := time.Now()
	screen.AddShow(startTime, movie)

	// We cannot directly access Screen.shows because it is private.
	// So create the Show separately for testing.
	show := heart.NewShow(screen, movie, 4)

	bookingService := bookingservice.NewBookingService()
	application := app.NewApp(bookingService)

	// --------------------------------------------------
	// Test 1: Simple successful booking
	// --------------------------------------------------

	fmt.Println("\n========== TEST 1 ==========")
	fmt.Println("Simple successful booking")

	ok := application.Book(
		500,
		"Alice",
		show,
		"s-11",
	)

	fmt.Println("Booking result:", ok)

	// --------------------------------------------------
	// Test 2: Concurrent booking of same seat
	// --------------------------------------------------

	fmt.Println("\n========== TEST 2 ==========")
	fmt.Println("Concurrent booking of same seat")

	// New show so that s-12 is available.
	show2 := heart.NewShow(screen, movie, 4)

	var wg sync.WaitGroup

	users := []string{
		"User-1",
		"User-2",
		"User-3",
		"User-4",
		"User-5",
		"User-6",
		"User-7",
		"User-8",
		"User-9",
		"User-10",
	}

	results := make(chan bool, len(users))

	for _, user := range users {
		wg.Add(1)

		go func(name string) {
			defer wg.Done()

			ok := application.Book(
				500,
				name,
				show2,
				"s-12",
			)

			results <- ok
		}(user)
	}

	wg.Wait()
	close(results)

	success := 0
	failed := 0

	for result := range results {
		if result {
			success++
		} else {
			failed++
		}
	}

	fmt.Println("Successful bookings:", success)
	fmt.Println("Failed bookings:", failed)

	// Exactly one user should succeed.
	if success == 1 {
		fmt.Println("PASS: Only one user booked the seat.")
	} else {
		fmt.Println("FAIL: More than one user booked the seat.")
	}

	// --------------------------------------------------
	// Test 3: Multiple seats atomicity
	// --------------------------------------------------

	fmt.Println("\n========== TEST 3 ==========")
	fmt.Println("Multiple seat booking with rollback")

	show3 := heart.NewShow(screen, movie, 4)

	// First user takes s-21.
	ok = application.Book(
		500,
		"Bob",
		show3,
		"s-21",
	)

	fmt.Println("Bob booking s-21:", ok)

	// Alice tries to book s-21 + s-22.
	// Since s-21 is already held/booked, the entire booking
	// should fail and s-22 should be rolled back.
	ok = application.Book(
		500,
		"Alice",
		show3,
		"s-21",
		"s-22",
	)

	fmt.Println("Alice booking s-21 + s-22:", ok)

	// Now Charlie tries s-22.
	// This should succeed if rollback worked correctly.
	ok = application.Book(
		500,
		"Charlie",
		show3,
		"s-22",
	)

	fmt.Println("Charlie booking s-22:", ok)

	if ok {
		fmt.Println("PASS: Failed multi-seat booking rolled back successfully.")
	} else {
		fmt.Println("FAIL: s-22 remained locked after rollback.")
	}

	// --------------------------------------------------
	// Test 4: Seat expiry
	// --------------------------------------------------

	fmt.Println("\n========== TEST 4 ==========")
	fmt.Println("Seat hold expiry")

	// Use a very short timeout.
	expiryShow := heart.NewShow(screen, movie, 2)

	// Directly hold the seat instead of paying.
	// This simulates a user selecting seats and then
	// abandoning payment.
	holdOK := expiryShow.GetShowSeats().Hold(
		"ExpiredUser",
		[]string{"s-31"},
	)

	fmt.Println("Seat hold:", holdOK)

	fmt.Println(
		"Current holder:",
		expiryShow.GetShowSeats().
			GetSeatStateById("s-31").
			WhoHeld(),
	)

	fmt.Println("Waiting for seat expiry...")

	time.Sleep(3 * time.Second)

	holder := expiryShow.GetShowSeats().
		GetSeatStateById("s-31").
		WhoHeld()

	fmt.Println("Holder after expiry:", holder)

	if holder == "" {
		fmt.Println("PASS: Seat expired and was released.")
	} else {
		fmt.Println("FAIL: Seat was not released.")
	}

	// --------------------------------------------------
	// Test 5: Booking after expiry
	// --------------------------------------------------

	fmt.Println("\n========== TEST 5 ==========")
	fmt.Println("Booking seat after previous user's expiry")

	ok = application.Book(
		500,
		"NewUser",
		expiryShow,
		"s-31",
	)

	fmt.Println("NewUser booking result:", ok)

	if ok {
		fmt.Println("PASS: Expired seat can be booked again.")
	} else {
		fmt.Println("FAIL: Expired seat could not be booked.")
	}

	// --------------------------------------------------
	// Test 6: Concurrent booking of different seats
	// --------------------------------------------------

	fmt.Println("\n========== TEST 6 ==========")
	fmt.Println("Concurrent booking of different seats")

	show4 := heart.NewShow(screen, movie, 4)

	var wg2 sync.WaitGroup

	seatIDs := []string{
		"s-11",
		"s-21",
		"s-31",
		"s-41",
		"s-51",
		"s-12",
		"s-22",
		"s-32",
		"s-42",
		"s-52",
	}

	for i, seatID := range seatIDs {
		wg2.Add(1)

		go func(i int, seatID string) {
			defer wg2.Done()

			user := fmt.Sprintf("Customer-%d", i+1)

			ok := application.Book(
				500,
				user,
				show4,
				seatID,
			)

			fmt.Printf(
				"%s -> %s -> %v\n",
				user,
				seatID,
				ok,
			)
		}(i, seatID)
	}

	wg2.Wait()

	fmt.Println("Finished concurrent different-seat test.")

	// --------------------------------------------------
	// Test 7: Concurrent multi-seat booking
	// --------------------------------------------------

	fmt.Println("\n========== TEST 7 ==========")
	fmt.Println("Concurrent multi-seat booking")

	show5 := heart.NewShow(screen, movie, 4)

	var wg3 sync.WaitGroup

	for i := 1; i <= 5; i++ {
		wg3.Add(1)

		go func(i int) {
			defer wg3.Done()

			user := fmt.Sprintf("Group-%d", i)

			ok := application.Book(
				1000,
				user,
				show5,
				"s-41",
				"s-42",
			)

			fmt.Printf(
				"%s -> s-41,s-42 -> %v\n",
				user,
				ok,
			)
		}(i)
	}

	wg3.Wait()

	// --------------------------------------------------
	// Final
	// --------------------------------------------------

	fmt.Println("\n========================================")
	fmt.Println("             TESTING DONE")
	fmt.Println("========================================")
}
