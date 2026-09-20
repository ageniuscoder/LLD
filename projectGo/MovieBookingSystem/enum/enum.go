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
	BOOKING_CREATED
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

type PaymentStatus int

const (
	_ PaymentStatus = iota
	PAYMENT_CREATED
	PAYMENT_SUCCESS
	PAYMENT_FAILED
)

func (p PaymentStatus) String() string {
	switch p {
	case PAYMENT_SUCCESS:
		return "succes"
	case PAYMENT_FAILED:
		return "failed"
	default:
		return "unknown"
	}
}
