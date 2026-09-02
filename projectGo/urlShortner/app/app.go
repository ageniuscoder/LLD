package app

import (
	"fmt"
	"strings"
	"urlshortner/analytics"
	"urlshortner/publisher"
	"urlshortner/shortner"
	"urlshortner/storage"
)

const prefix = "mangal/"

type UrlShortner struct {
	urlshortner *shortner.ShortnerService
	storage     storage.Storage
	publisher   publisher.EventPublisher
}

func NewUrlShortner(urlshortner *shortner.ShortnerService, storage storage.Storage, publisher publisher.EventPublisher) *UrlShortner {
	return &UrlShortner{
		urlshortner,
		storage,
		publisher,
	}
}

func (u *UrlShortner) ShortUrl(url string) (string, error) {
	data, err := u.urlshortner.ShortUrl(url)
	if err != nil {
		return "", err
	}
	u.storage.Store(data)
	return prefix + data.GetShortCode(), nil
}

func (u *UrlShortner) ResolveUrl(shortUrl string) (string, error) {
	if !strings.HasPrefix(shortUrl, prefix) {
		return "", fmt.Errorf("invalid short URL")
	}
	code := strings.TrimPrefix(shortUrl, prefix)
	data, err := u.storage.Get(code)
	if err != nil {
		return "", err
	}
	if err := u.publisher.Publish(analytics.NewClickEvent(code, "192.168.54.32")); err != nil {
		fmt.Println(err.Error()) //only logging because to analytics failure i can,t stop resolving url
	}
	return data.GetLongUrl(), nil
}
