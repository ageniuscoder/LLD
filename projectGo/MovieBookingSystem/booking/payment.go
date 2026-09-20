package booking

import (
	"bookmyshow/enum"
)

type Payment struct {
	amount int
	status enum.PaymentStatus
}

func NewPayment(amount int, status enum.PaymentStatus) *Payment {
	return &Payment{
		amount: amount,
		status: status,
	}
}

func (p *Payment) GetStatus() string {
	return p.status.String()
}
