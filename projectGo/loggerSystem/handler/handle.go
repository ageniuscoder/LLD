package handler

import (
	"fmt"
	"logger/appender"
	"logger/logmsg"
	"os"
	"sync"
)

type LogHandler interface{
	HandleLog(msg *logmsg.LogMsg)
	HandleBatch(msgs []*logmsg.LogMsg)
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
func (bh *BaseHandler) Notify(msg *logmsg.LogMsg)  {  //imp
	bh.mu.RLock()
	// Copy slice under read lock to avoid holding lock during I/O
	appenders := make([]appender.LogAppender, len(bh.appenders))
	copy(appenders, bh.appenders)
	bh.mu.RUnlock()

	for _,ap:=range appenders{   
		err:=ap.AppendMsg(msg)
		if err!=nil{
			fmt.Fprintf(os.Stderr, "logger: appender error: %v\n", err)   //fixed here
		}
	}
}

func (bh *BaseHandler) NotifyBatch(msgs []*logmsg.LogMsg) {
	bh.mu.RLock()
	appenders := make([]appender.LogAppender, len(bh.appenders))
	copy(appenders, bh.appenders)
	bh.mu.RUnlock()

	for _,ap:=range appenders{
		if err:=ap.AppendBatch(msgs); err!=nil{
			fmt.Fprintf(os.Stderr,"logger: appender error: %v\n", err)
		}
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

func (bh *BaseHandler) ForwardBatch(msgs []*logmsg.LogMsg){
	bh.mu.RLock()  
	next := bh.next
	bh.mu.RUnlock()

	if next!=nil{
		next.HandleBatch(msgs)
	}
}

func (bh *BaseHandler) GetMineAndOtherBatch(level logmsg.LogLevel,msgs []*logmsg.LogMsg) ([]*logmsg.LogMsg,[]*logmsg.LogMsg){
	mine:=make([]*logmsg.LogMsg,0)
	other:=make([]*logmsg.LogMsg,0)

	for _,msg:=range msgs{
		if msg.Level==level{
			mine = append(mine, msg)
		}else{
			other = append(other, msg)
		}
	}

	return mine,other

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

func (dh *DebugHandler) HandleBatch(msgs []*logmsg.LogMsg){
	mine,other:=dh.GetMineAndOtherBatch(logmsg.DEBUG,msgs)

	if len(mine)>0{
		dh.NotifyBatch(mine)
	}
	dh.ForwardBatch(other)
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

func (ih *InfoHandler) HandleBatch(msgs []*logmsg.LogMsg){
	mine,other:=ih.GetMineAndOtherBatch(logmsg.INFO,msgs)

	if len(mine)>0{
		ih.NotifyBatch(mine)
	}
	ih.ForwardBatch(other)
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

func (wh *WarningHandler) HandleBatch(msgs []*logmsg.LogMsg){
	mine,other:=wh.GetMineAndOtherBatch(logmsg.WARNING,msgs)

	if len(mine)>0{
		wh.NotifyBatch(mine)
	}
	wh.ForwardBatch(other)
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

func (eh *ErrorHandler) HandleBatch(msgs []*logmsg.LogMsg){
	mine,other:=eh.GetMineAndOtherBatch(logmsg.ERROR,msgs)

	if len(mine)>0{
		eh.NotifyBatch(mine)
	}
	eh.ForwardBatch(other)
}
