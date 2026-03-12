package handler

import (
	"logger/appender"
	"logger/logmsg"
	"sync"
)

type LogHandler interface{
	HandleLog(msg *logmsg.LogMsg)
	SetNext(next LogHandler)
	AddAppender(appender appender.LogAppender)
	// Notify(msg *logmsg.LogMsg)
	// Forward(msg *logmsg.LogMsg)
}

type BaseHandler struct{
	mu sync.RWMutex
	next LogHandler
	appenders []appender.LogAppender
}

func (bh *BaseHandler) SetNext(next LogHandler){
	bh.mu.Lock()
	defer bh.mu.Unlock()
	bh.next=next
}

func (bh *BaseHandler) AddAppender(appender appender.LogAppender){
	bh.mu.Lock()
	defer bh.mu.Unlock()
	bh.appenders=append(bh.appenders, appender)
}

// Notify fans out to all appenders concurrently.
// The slice is copied under RLock so the lock isn't held during I/O.
func (bh *BaseHandler) Notify(msg *logmsg.LogMsg){  //imp
	bh.mu.RLock()
	// Copy slice under read lock to avoid holding lock during I/O
	appenders := make([]appender.LogAppender, len(bh.appenders))
	copy(appenders, bh.appenders)
	bh.mu.RUnlock()

	for _,ap:=range appenders{   
		ap.AppendMsg(msg)
	}
}
// Forward reads next under RLock into a local variable, then calls it.
// This avoids reading bh.next a second time outside the lock.
func (bh *BaseHandler) Forward(msg *logmsg.LogMsg){
	bh.mu.RLock()  //same logic here as above
	next := bh.next
	bh.mu.RUnlock()

	if next!=nil{
		next.HandleLog(msg)
	}
}

type DebugHandler struct{
	BaseHandler
}

func NewDebugHandler() *DebugHandler{
	return &DebugHandler{}
}

func (dh *DebugHandler) HandleLog(msg *logmsg.LogMsg){
	if msg.Level==logmsg.DEBUG{
		dh.Notify(msg)
		return
	}
	dh.Forward(msg)
}

type InfoHandler struct{
	BaseHandler
}

func NewInfoHandler() *InfoHandler{
	return &InfoHandler{}
}

func (ih *InfoHandler) HandleLog(msg *logmsg.LogMsg){
	if msg.Level==logmsg.INFO{
		ih.Notify(msg)
		return
	}
	ih.Forward(msg)
}

type WarningHandler struct{
	BaseHandler
}

func NewWarningHandler() *WarningHandler{
	return &WarningHandler{}
}

func (wh *WarningHandler) HandleLog(msg *logmsg.LogMsg){
	if msg.Level==logmsg.WARNING{
		wh.Notify(msg)
		return
	}
	wh.Forward(msg)
}

type ErrorHandler struct{
	BaseHandler
}

func NewErrorHandler() *ErrorHandler{
	return &ErrorHandler{}
}

func (eh *ErrorHandler) HandleLog(msg *logmsg.LogMsg){
	if msg.Level==logmsg.ERROR{
		eh.Notify(msg)
		return
	}
	eh.Forward(msg)
}
