package main

import (
	"fmt"
	"sync"

	"urlshortner/app"
	gen "urlshortner/shortcodegen"
	"urlshortner/shortner"
	"urlshortner/storage"
)

func main() {
	// -------------------------------
	// Setup
	// -------------------------------

	b62 := gen.NewBase62Gen()
	shortnerService := shortner.NewShortnerService(b62)
	mem := storage.NewInMemStorage()

	service := app.NewUrlShortner(shortnerService, mem)

	// -------------------------------
	// Test 1: Basic functionality
	// -------------------------------

	fmt.Println("===== Basic Test =====")

	url := "amazon.com/kskskk/jfhososos"

	shortURL, err := service.ShortUrl(url)
	if err != nil {
		fmt.Println("Shorten error:", err)
		return
	}

	fmt.Println("Original:", url)
	fmt.Println("Short:", shortURL)

	longURL, err := service.ResolveUrl(shortURL)
	if err != nil {
		fmt.Println("Resolve error:", err)
		return
	}

	fmt.Println("Resolved:", longURL)

	if longURL != url {
		fmt.Println("❌ Basic test failed")
		return
	}

	fmt.Println("✅ Basic test passed")

	// -------------------------------
	// Test 2: Concurrent Shortening
	// -------------------------------

	fmt.Println("\n===== Concurrent Shortening Test =====")

	const n = 1000

	var wg sync.WaitGroup

	shortURLs := make(chan string, n)
	errors := make(chan error, n)

	wg.Add(n)

	for i := 0; i < n; i++ {
		go func(i int) {
			defer wg.Done()

			url := fmt.Sprintf(
				"https://example.com/page/%d",
				i,
			)

			shortURL, err := service.ShortUrl(url)

			if err != nil {
				errors <- err
				return
			}

			shortURLs <- shortURL
		}(i)
	}

	wg.Wait()

	close(shortURLs)
	close(errors)

	if len(errors) > 0 {
		fmt.Println("❌ Concurrent shortening failed")

		for err := range errors {
			fmt.Println(err)
		}

		return
	}

	// -------------------------------
	// Check uniqueness
	// -------------------------------

	seen := make(map[string]bool)

	for shortURL := range shortURLs {
		if seen[shortURL] {
			fmt.Println("❌ Duplicate short code:", shortURL)
			return
		}

		seen[shortURL] = true
	}

	if len(seen) != n {
		fmt.Printf(
			"❌ Expected %d unique codes, got %d\n",
			n,
			len(seen),
		)
		return
	}

	fmt.Printf("Generated %d unique short URLs\n", len(seen))
	fmt.Println("✅ Concurrent shortening passed")

	// -------------------------------
	// Test 3: Concurrent Resolve
	// -------------------------------

	fmt.Println("\n===== Concurrent Resolve Test =====")

	var resolveWG sync.WaitGroup

	resolveErrors := make(chan error, n)

	for shortURL := range seen {
		resolveWG.Add(1)

		go func(shortURL string) {
			defer resolveWG.Done()

			_, err := service.ResolveUrl(shortURL)

			if err != nil {
				resolveErrors <- err
			}
		}(shortURL)
	}

	resolveWG.Wait()

	close(resolveErrors)

	if len(resolveErrors) > 0 {
		fmt.Println("❌ Concurrent resolve failed")

		for err := range resolveErrors {
			fmt.Println(err)
		}

		return
	}

	fmt.Println("✅ Concurrent resolve passed")

	// -------------------------------
	// Final
	// -------------------------------

	fmt.Println("\n==============================")
	fmt.Println("ALL TESTS PASSED")
	fmt.Println("==============================")
}
