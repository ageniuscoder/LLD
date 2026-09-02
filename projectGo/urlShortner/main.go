package main

import (
	"fmt"
	"time"

	"urlshortner/analytics"
	"urlshortner/app"
	"urlshortner/publisher"
	gen "urlshortner/shortcodegen"
	"urlshortner/shortner"
	"urlshortner/storage"
	"urlshortner/worker"
)

func main() {
	// Dependencies
	generator := gen.NewBase62Gen()
	shortnerService := shortner.NewShortnerService(generator)
	store := storage.NewInMemStorage()

	eventPublisher := publisher.NewChannelPublisher(100)
	analyticsService := analytics.NewInMemAnalytics()

	eventWorker := worker.NewChannelWorker(
		eventPublisher.GetQueue(),
		analyticsService,
	)

	// Start worker
	go eventWorker.Consume()

	// Application
	service := app.NewUrlShortner(
		shortnerService,
		store,
		eventPublisher,
	)

	// Multiple URLs
	urls := []string{
		"https://google.com",
		"https://youtube.com",
		"https://github.com",
	}

	shortURLs := make([]string, 0)

	// Create short URLs
	for _, url := range urls {
		shortURL, err := service.ShortUrl(url)
		if err != nil {
			fmt.Println("Error:", err)
			return
		}

		shortURLs = append(shortURLs, shortURL)

		fmt.Println(url, "->", shortURL)
	}

	fmt.Println("\nResolving URLs...")

	// Simulate clicks
	// Google   -> 3 clicks
	// YouTube  -> 2 clicks
	// GitHub   -> 5 clicks

	for i := 0; i < 3; i++ {
		service.ResolveUrl(shortURLs[0])
	}

	for i := 0; i < 2; i++ {
		service.ResolveUrl(shortURLs[1])
	}

	for i := 0; i < 5; i++ {
		service.ResolveUrl(shortURLs[2])
	}

	// Give worker time to process events
	time.Sleep(100 * time.Millisecond)

	// Check analytics
	fmt.Println("\nAnalytics:")
	for _, shortURL := range shortURLs {
		fmt.Println(
			shortURL,
			"clicks:",
			analyticsService.ClickCount(
				shortURL[len("mangal/"):],
			),
		)
	}

	fmt.Println(
		"Total clicks:",
		analyticsService.TotalClickCount(),
	)
}
