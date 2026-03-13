package logmsg

type LogLevel int

const (
	_ LogLevel = iota
	DEBUG
	INFO
	WARNING
	ERROR
	FATAL
)

func ParseLevel(s string) (LogLevel, bool) {
	switch s {
	case "debug":
		return DEBUG, true
	case "info":
		return INFO, true
	case "warning":
		return WARNING, true
	case "error":
		return ERROR, true
	case "fatal":
		return FATAL, true
	}
	return 0, false
}
func (l LogLevel) ToStr() string {
	switch l {
	case DEBUG:
		return "debug"
	case INFO:
		return "info"
	case WARNING:
		return "warning"
	case ERROR:
		return "error"
	case FATAL:
		return "fatal"
	}
	return "unknown"
}