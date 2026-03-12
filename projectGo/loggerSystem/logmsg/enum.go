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