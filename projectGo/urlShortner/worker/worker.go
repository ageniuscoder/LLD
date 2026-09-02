package worker

import "urlshortner/analytics"

type Worker interface {
	Consume() error
}

type ChannelWorker struct {
	queue            chan *analytics.ClickEvent
	analyticsService analytics.AnalyticsService
}

func NewChannelWorker(queue chan *analytics.ClickEvent, analyticsService analytics.AnalyticsService) *ChannelWorker {
	return &ChannelWorker{
		queue,
		analyticsService,
	}
}

func (c *ChannelWorker) Consume() error {
	for data := range c.queue {
		if err := c.analyticsService.RecordClick(data); err != nil {
			return err
		}
	}
	return nil
}
