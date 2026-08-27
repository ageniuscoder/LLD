package handler

import "fmt"

type Handler interface {
	Dispense(amount int)
	SetNext(n Handler)
}

type handler struct {
	htype int
	notes int
	next  Handler
}

func (h *handler) SetNext(n Handler) {
	h.next = n
}

func (h *handler) Dispense(amount int) {
	required := amount / h.htype

	dispensed := min(required, h.notes)
	h.notes -= dispensed

	fmt.Printf("%d x %d dispensed\n", dispensed, h.htype)

	remaining := amount - dispensed*h.htype

	if remaining > 0 {
		if h.next != nil {
			h.next.Dispense(remaining)
		} else {
			fmt.Printf("Cannot dispense remaining ₹%d\n", remaining)
		}
	}
}

type ThousandHandler struct {
	handler
}

func NewThousandHandler(notes int) *ThousandHandler {
	return &ThousandHandler{
		handler: handler{
			htype: 1000,
			notes: notes,
			next:  nil,
		},
	}
}

type TwoHundredHandler struct {
	handler
}

func NewTwoHundredHandler(notes int) *TwoHundredHandler {
	return &TwoHundredHandler{
		handler: handler{
			htype: 200,
			notes: notes,
			next:  nil,
		},
	}
}

type HundredHandler struct {
	handler
}

func NewHundredHandler(notes int) *HundredHandler {
	return &HundredHandler{
		handler: handler{
			htype: 100,
			notes: notes,
			next:  nil,
		},
	}
}
