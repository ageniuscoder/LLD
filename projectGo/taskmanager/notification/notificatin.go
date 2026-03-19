package notification

import "fmt"

type Notifiler interface{
	AssignUpdate(cid,uid, tid string)
	CompleteUpdate(cid,uid,tid string)
}

type InAppNotification struct{}

func NewInAppNotification() *InAppNotification {
	return &InAppNotification{}
}

func (ns *InAppNotification) AssignUpdate(cid,uid, tid string) {
	fmt.Printf("[Notification to %s]: %s is assigned\n",uid,tid)
}

func (ns *InAppNotification) CompleteUpdate(cid,uid,tid string){
	fmt.Printf("[Notification to %s]:%s has completed the task %s\n",cid,uid,tid)
}