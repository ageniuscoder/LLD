package enum

type SeatStatus int

const (
	_ SeatStatus = iota
	SEAT_AVAIL
	SEAT_HELD
	SEAT_BOOKED
)

func (s SeatStatus) String() string {
	switch s {
	case SEAT_AVAIL:
		return "avail"
	case SEAT_HELD:
		return "held"
	case SEAT_BOOKED:
		return "booked"
	default:
		return "unknown"
	}
}

type BookingStatus int

const (
	_ BookingStatus = iota
	BOOKING_CONFIRMED
	BOOKING_FAILED
)

func (b BookingStatus) String() string {
	switch b {
	case BOOKING_CONFIRMED:
		return "booked"
	case BOOKING_FAILED:
		return "failed"
	default:
		return "unknown"
	}
}

type PaymentStaus int

const (
	_ PaymentStaus = iota
	PAYMENT_SUCCESS
	PAYMENT_FAILED
)

func (p PaymentStaus) String() string {
	switch p {
	case PAYMENT_SUCCESS:
		return "succes"
	case PAYMENT_FAILED:
		return "failed"
	default:
		return "unknown"
	}
}
