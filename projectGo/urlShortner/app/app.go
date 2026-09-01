package app

import (
	"fmt"
	"strings"
	"urlshortner/shortner"
	"urlshortner/storage"
)

const prefix = "mangal/"

type UrlShortner struct {
	urlshortner *shortner.ShortnerService
	storage     storage.Storage
}

func NewUrlShortner(urlshortner *shortner.ShortnerService, storage storage.Storage) *UrlShortner {
	return &UrlShortner{
		urlshortner,
		storage,
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
	return data.GetLongUrl(), nil
}
