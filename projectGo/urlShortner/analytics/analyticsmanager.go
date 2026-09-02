package analytics

import "sync"

type AnalyticsService interface {
	RecordClick(*ClickEvent) error
	ClickCount(shortCode string) int
	TotalClickCount() int
}

type InMemAnalytics struct {
	events map[string][]*ClickEvent
	mu     sync.Mutex
}

func NewInMemAnalytics() *InMemAnalytics {
	return &InMemAnalytics{
		events: make(map[string][]*ClickEvent),
	}
}

func (this *InMemAnalytics) RecordClick(c *ClickEvent) error {
	this.mu.Lock()
	defer this.mu.Unlock()
	this.events[c.GetShortCode()] = append(this.events[c.GetShortCode()], c)
	return nil
}

func (this *InMemAnalytics) ClickCount(shortCode string) int {
	this.mu.Lock()
	defer this.mu.Unlock()
	return len(this.events[shortCode])
}

func (this *InMemAnalytics) TotalClickCount() int {
	this.mu.Lock()
	defer this.mu.Unlock()
	cnt := 0
	for _, e := range this.events {
		cnt += len(e)
	}
	return cnt
}
