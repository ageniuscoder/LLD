package booking

import (
	"bookmyshow/enum"
	"bookmyshow/heart"
)

type Booking struct {
	user    string
	show    *heart.Show
	payment *Payment
	status  enum.BookingStatus
	seats   []string
}

func NewBooking(name string, show *heart.Show, seats []string) *Booking {
	return &Booking{
		user:   name,
		show:   show,
		status: enum.BOOKING_CREATED,
		seats:  seats,
	}
}

func (b *Booking) AddPayment(p *Payment) {
	b.payment = p
}

func (b *Booking) GetSeats() []string {
	return b.seats
}

func (b *Booking) UpdateStatus(s enum.BookingStatus) {
	b.status = s
}

func (b *Booking) GetShowSeats() *heart.ShowSeat {
	return b.show.GetShowSeats()
}

func (b *Booking) GetUser() string {
	return b.user
}
