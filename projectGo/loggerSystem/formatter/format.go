package formatter

import (
	"encoding/json"
	"fmt"
	"logger/logmsg"
)

type LogFormatter interface {
	Format(msg *logmsg.LogMsg) string
}

type TextFormatter struct{
}

func NewTextFormatter() *TextFormatter{
	return &TextFormatter{
	}
}

func (tf *TextFormatter) Format(msg *logmsg.LogMsg) string{
	return fmt.Sprintf("[%s]- %s: %s",msg.GetLevel(),msg.GetTimestamp(),msg.GetContent())
}

type JsonFormatter struct{
}

func NewJsonFormatter() *JsonFormatter{
	return &JsonFormatter{
	}
}

func (jf *JsonFormatter) Format(msg *logmsg.LogMsg) string {
	// Create a local map or anonymous struct to avoid shared state mutation
	data, _ := json.Marshal(struct {
		Timestamp string          `json:"timestamp"`
		Level     string          `json:"level"`
		Content   string          `json:"content"`
	}{
		Timestamp: msg.GetTimestamp(),
		Level:     msg.GetLevel(),
		Content:   msg.GetContent(),
	})
	return string(data)
}