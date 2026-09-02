package analytics

import "time"

type ClickEvent struct {
	shortCode string
	timestamp time.Time
	ip        string
}

func NewClickEvent(shortCode, ip string) *ClickEvent {
	return &ClickEvent{
		shortCode: shortCode,
		timestamp: time.Now(),
		ip:        ip,
	}
}

func (c *ClickEvent) GetShortCode() string {
	return c.shortCode
}
