package notification

import "fmt"

type Notifier interface {
	Update(uid string, itemid string, newPrice int)
}

type InAppNotification struct {
}

func NewInAppNotification() *InAppNotification {
	return &InAppNotification{}
}

func (in *InAppNotification) Update(uid string, itemid string, newPrice int) {
	fmt.Printf("[InAppNotification]:[Notification to %s]:%s new bid %d\n",uid,itemid,newPrice)
}

type EmailNotification struct {
}

func NewEmailNotification() *EmailNotification {
	return &EmailNotification{}
}

func (in *EmailNotification) Update(uid string, itemid string, newPrice int) {
	fmt.Printf("[EmailNotification]:[Notification to %s]:%s new bid %d\n",uid,itemid,newPrice)
}