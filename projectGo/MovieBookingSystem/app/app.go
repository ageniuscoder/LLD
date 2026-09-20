package app

import (
	"bookmyshow/booking"
	"bookmyshow/bookingservice"
	"bookmyshow/enum"
	"bookmyshow/heart"
)

type App struct {
	bookingservice *bookingservice.BookingService
}

func NewApp(srv *bookingservice.BookingService) *App {
	return &App{
		bookingservice: srv,
	}
}

func pay(book *booking.Booking, amount int) bool {
	if book == nil {
		return false
	}
	var shouldPay bool = true
	for _, seat := range book.GetSeats() {
		if book.GetShowSeats().GetSeatStateById(seat).WhoHeld() != book.GetUser() {
			shouldPay = false
			break
		}
	}
	if !shouldPay { //expired seats get unlocked
		book.AddPayment(booking.NewPayment(amount, enum.PAYMENT_FAILED))
		return false
	}
	book.AddPayment(booking.NewPayment(amount, enum.PAYMENT_SUCCESS))
	return true
}

func (a *App) Book(amount int, name string, show *heart.Show, seats ...string) bool {
	book := a.bookingservice.CreateBooking(show, name, seats)
	if ok := pay(book, amount); ok {
		a.bookingservice.ConfirmBooking(book)
		return true
	}

	return false
}
