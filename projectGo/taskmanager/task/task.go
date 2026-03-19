package task

import (
	"strconv"
	"sync/atomic"
	enum "taskmanager/enums"
	"time"
)

var Tid int64

type Task struct {
	tid string
	title       string
	description string
	dueDate     time.Time
	priority enum.Priority
	status enum.TaskStatus
	createdBy  string    //uid
	assignedTo string   //uid
}


type TaskBuilder struct{
	task Task
}

          
func NewTaskBuilder() titleStep {
	return &TaskBuilder{}
}

         //Step Builder

type optionalStep interface{
	WithPriority(p enum.Priority) optionalStep
	WithDescription(d string) optionalStep
	WithAssignedTo(id string) optionalStep
	Build() Task
}

type createdByStep interface{
	WithCreatedBy(id string) optionalStep
}

type dueDateStep interface{
	WithDueDate(days int) createdByStep
}

type titleStep interface{
	WithTitle(t string) dueDateStep
}

func (tb *TaskBuilder) WithTitle(title string) dueDateStep {
	tb.task.title = title
	return tb
}

func (tb *TaskBuilder) WithDueDate(days int) createdByStep {
	tb.task.dueDate = time.Now().Add(time.Duration(days) * 24 * time.Hour)
	return tb
}

func (tb *TaskBuilder) WithCreatedBy(id string) optionalStep{
	tb.task.createdBy=id
	return tb
}

func (tb *TaskBuilder) WithDescription(des string) optionalStep {
	tb.task.description = des
	return tb
}

func (tb *TaskBuilder) WithPriority(p enum.Priority) optionalStep {
	tb.task.priority = p
	return tb
}

func (tb *TaskBuilder) WithAssignedTo(id string) optionalStep{
	tb.task.assignedTo=id
	return tb
}

func (tb *TaskBuilder) Build() Task {
	t := tb.task
	t.status = enum.PENDING
	t.tid="task"+strconv.FormatInt(atomic.AddInt64(&Tid,1),10)
	tb.task = Task{} // reset builder (important)
	return t
}

			//Simple builder
// func (tb *TaskBuilder) WithTitle(title string) *TaskBuilder {
// 	tb.task.title = title
// 	return tb
// }

// func (tb *TaskBuilder) WithDescription(des string) *TaskBuilder {
// 	tb.task.description = des
// 	return tb
// }

// func (tb *TaskBuilder) WithDueIn(days int) *TaskBuilder {
// 	tb.task.dueDate = time.Now().Add(time.Duration(days) * 24 * time.Hour)
// 	return tb
// }

// func (tb *TaskBuilder) WithDueDate(t time.Time) *TaskBuilder {
// 	tb.task.dueDate = t
// 	return tb
// }

// func (tb *TaskBuilder) WithPriority(p enum.Priority) *TaskBuilder {
// 	tb.task.priority = p
// 	return tb
// }

// func (tb *TaskBuilder) Build() (Task, error) {
// 	if tb.task.title == "" {
// 		return Task{}, fmt.Errorf("title is required")
// 	}

// 	t := tb.task
// 	tb.task = Task{} // reset builder

// 	return t, nil
// }


func (t *Task) AddAssignee(id string) {
	t.assignedTo = id
}
 
func (t *Task) GetPriority() enum.Priority {
	return t.priority
}
 
func (t *Task) GetDueDate() time.Time {
	return t.dueDate
}
 
func (t *Task) GetId() string {
	return t.tid
}
 
func (t *Task) GetTitle() string {
	return t.title
}
 
func (t *Task) GetStatus() enum.TaskStatus {
	return t.status
}
 
func (t *Task) ChangeStatus(st enum.TaskStatus) {
	t.status = st
}
 
// GetCid returns the creator's user ID.
func (t *Task) GetCid() string {
	return t.createdBy
}
 
// GetAssignedTo returns the assignee's user ID.
func (t *Task) GetAssignedTo() string {
	return t.assignedTo
}