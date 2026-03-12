package logmsg

import "time"

type LogMsg struct {
	Timestamp time.Time
	Level LogLevel
	Content string
}

func NewLogMsg(level LogLevel,content string) *LogMsg{
	return &LogMsg{
		Timestamp: time.Now(),
		Level: level,
		Content: content,
	}
}

func (lm *LogMsg) GetTimestamp() string{
	st:=lm.Timestamp.Format("2006-01-02 15:04:05")
	return st
}

func (lm *LogMsg) GetContent() string{
	return lm.Content
}

func (lm *LogMsg) GetLevel() string{
	return lm.Level.ToStr()
}