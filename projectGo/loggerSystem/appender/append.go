package appender

import (
	"fmt"
	"logger/formatter"
	"logger/logmsg"
	"os"
	"sync"
)

type LogAppender interface{
	AppendMsg(msg *logmsg.LogMsg) error
}

type ConsoleAppender struct{
	formatter formatter.LogFormatter
}

func NewConsoleAppender(formatter formatter.LogFormatter) *ConsoleAppender{
	return &ConsoleAppender{
		formatter: formatter,
	}
}

func (ca *ConsoleAppender) AppendMsg(msg *logmsg.LogMsg) error {
	m:=ca.formatter.Format(msg)
	fmt.Println(m)
	return nil
}

type FileAppender struct{
	mu sync.Mutex
	file *os.File
	formatter formatter.LogFormatter
}

func NewFileAppender(path string,formatter formatter.LogFormatter) (*FileAppender,error){
	file,err:=os.OpenFile(
		path,
		os.O_APPEND |os.O_CREATE|os.O_WRONLY,
		0644,
	)
	if err!=nil{
		return nil,err
	}
	fa:= &FileAppender{
		file: file,
		formatter: formatter,
	}
	return fa,nil
}

func (fa *FileAppender) AppendMsg(msg *logmsg.LogMsg) error {
	m:=fa.formatter.Format(msg)
	fa.mu.Lock()
	defer fa.mu.Unlock()
	_,err:=fa.file.WriteString(m+"\n")
	if err!=nil{
		return fmt.Errorf("Appender: can,t write log to file: %w",err)
	}
	return nil
}

func (fa *FileAppender) CloseFile(){
	fa.mu.Lock()
	defer fa.mu.Unlock()
	fa.file.Close()
}