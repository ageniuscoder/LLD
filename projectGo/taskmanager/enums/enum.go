package enum

type TaskStatus int

const (
	_ TaskStatus = iota
	PENDING
	INPROGRESS
	COMPLETED
)

type Priority int

const (
	_ Priority = iota
	LOW
	MEDIUM
	HIGH
)

type Action int

const (
	_ Action = iota
	TODO
	DONE
)

func (a Action) ToStr() string {
	switch a {
	case TODO:
		return "todo"
	case DONE:
		return "completed"
	default:
		return "unknown"
	}
}