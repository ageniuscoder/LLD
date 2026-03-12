package logs

import (
	"logger/appender"
	"logger/handler"
	"logger/logmsg"
	"sync"
)

//singleton
type Logger struct {
	handlers map[string]handler.LogHandler
	head handler.LogHandler
	logbuffer chan *logmsg.LogMsg
	wg sync.WaitGroup
	done chan struct{}
}

var(
	instance *Logger
	once sync.Once
)

func GetInstance() *Logger{
	once.Do(func() {
		mp:=make(map[string]handler.LogHandler)
		mp["debug"]=handler.NewDebugHandler()
		mp["info"]=handler.NewInfoHandler()
		mp["warning"]=handler.NewWarningHandler()
		mp["error"]=handler.NewErrorHandler()
		instance=&Logger{
			handlers: mp,
			logbuffer: make(chan *logmsg.LogMsg,512),
			done: make(chan struct{}),
		}
		instance.handlers["debug"].SetNext(instance.handlers["info"])
		instance.handlers["info"].SetNext(instance.handlers["warning"])
		instance.handlers["warning"].SetNext(instance.handlers["error"])
		instance.handlers["error"].SetNext(nil)
		instance.head=instance.handlers["debug"]
		instance.wg.Add(1)
		go instance.worker()
	})
	return instance
}

// worker is the single background goroutine that drains the channel.
// Single worker = guaranteed FIFO ordering of all log messages.
func (l *Logger) worker(){
	defer l.wg.Done()
	for{
		select {
		case msg,ok:=<-l.logbuffer:
			if !ok{
				return
			}
			l.head.HandleLog(msg)
		case <-l.done:     //logger is shutdown drain the buffer,bcz logbuffer is never closed
		for{
			select{
			case msg:=<-l.logbuffer:
				l.head.HandleLog(msg)
			default:
				return
			}
		}
		}
	
	}
}

func (l *Logger) Shutdown(){
	close(l.done)   // signals: reject new sends 
	//close(l.logbuffer)  don,t close data sending channel at recieving side,, only close at sender side
	l.wg.Wait()
}

func (l *Logger) AddAppender(level string,appender appender.LogAppender){
	if ap,ok:=l.handlers[level]; ok{
		ap.AddAppender(appender)
	}
}

func (l *Logger) log(level logmsg.LogLevel,msg string){
	m:=logmsg.NewLogMsg(level,msg)
	select{
	case <-l.done:
		//ignore silently if shutdown not panic
	case l.logbuffer<-m:
	}
	
}

func (l *Logger) Debug(con string){
	l.log(logmsg.DEBUG,con)
}

func (l *Logger) Info(con string){
	l.log(logmsg.INFO,con)
}

func (l *Logger) Warning(con string){
	l.log(logmsg.WARNING,con)
}

func (l *Logger) Error(con string){
	l.log(logmsg.ERROR,con)
}



