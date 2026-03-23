package enum

type AuctionStatus int

const (
	_ AuctionStatus = iota
	PENDING
	ACTIVE
	CLOSED
)
