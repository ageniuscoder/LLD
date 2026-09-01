package models

import "time"

type UrlMapper struct {
	longUrl    string
	shortCode  string
	created_at time.Time
}

func NewUrlMapper(longUrl string, shortUrl string) UrlMapper {
	return UrlMapper{
		longUrl:    longUrl,
		shortCode:  shortUrl,
		created_at: time.Now(),
	}
}

func (this UrlMapper) GetShortCode() string {
	return this.shortCode
}

func (this UrlMapper) GetLongUrl() string {
	return this.longUrl
}
