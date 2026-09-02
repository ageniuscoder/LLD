package publisher

import (
	"fmt"
	"urlshortner/analytics"
)

type EventPublisher interface {
	Publish(*analytics.ClickEvent) error
}

type ChannelPublisher struct {
	queue chan *analytics.ClickEvent
}

func NewChannelPublisher(buffer int) *ChannelPublisher {
	return &ChannelPublisher{
		queue: make(chan *analytics.ClickEvent, buffer),
	}
}

func (c *ChannelPublisher) Publish(a *analytics.ClickEvent) error { //it should be not blocking because analytics is not critical,it  idelly should not slow down resolve
	select {
	case c.queue <- a:
		return nil
	default:
		return fmt.Errorf("can,t publish event channel is full")
	}
}

func (c *ChannelPublisher) GetQueue() chan *analytics.ClickEvent {
	return c.queue
}
