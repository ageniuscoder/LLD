package bookingservice

import (
	"bookmyshow/booking"
	"bookmyshow/enum"
	"bookmyshow/heart"
)

type BookingService struct {
}

func NewBookingService() *BookingService {
	return &BookingService{}
}

func (b *BookingService) CreateBooking(show *heart.Show, name string, seats []string) *booking.Booking {
	if ok := show.GetShowSeats().Hold(name, seats); ok {
		nb := booking.NewBooking(name, show, seats)
		return nb
	}
	return nil
}

func (b *BookingService) ConfirmBooking(booking *booking.Booking) {
	booking.UpdateStatus(enum.BOOKING_CONFIRMED)
	for _, seat := range booking.GetSeats() {
		booking.GetShowSeats().GetSeatStateById(seat).MarkBooked()
	}

}
