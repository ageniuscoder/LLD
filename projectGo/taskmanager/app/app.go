package app

import (
	"fmt"
	enum "taskmanager/enums"
	"taskmanager/search"
	"taskmanager/task"
	"taskmanager/user"
)

// TaskApp orchestrates the interaction between UserManager and TaskManager.
// Both managers are injected via constructor (Dependency Injection).
type TaskApp struct {
	userManager *user.UserManager
	taskManager *task.TaskManager
}

// NewTaskApp accepts both managers as dependencies — classic DI pattern.
func NewTaskApp(um *user.UserManager, tm *task.TaskManager) *TaskApp {
	return &TaskApp{
		userManager: um,
		taskManager: tm,
	}
}

// ─── User Operations ─────────────────────────────────────────────────────────

// RegisterUser creates and registers a new user; returns the created User.
func (a *TaskApp) RegisterUser(name, email string) *user.User {
	u := user.NewUser(name, email)
	a.userManager.AddUser(u)
	fmt.Printf("[App]: User '%s' registered with ID=%s\n", name, u.GetId())
	return u
}

// ─── Task Operations ─────────────────────────────────────────────────────────

// CreateTask creates a task owned by adminId and returns its generated ID.
func (a *TaskApp) CreateTask(adminId, title, desc string, days int, priority enum.Priority) string {
	tid := a.taskManager.CreateTask(adminId, title, desc, days, priority)
	return tid
}

// AssignTask assigns an existing task (by ID) to a user (by ID).
func (a *TaskApp) AssignTask(adminId, userId, tid string) {
	a.taskManager.AssignTask(userId, tid)
	//fmt.Printf("[App]: Task %s assigned to user %s by admin %s\n", tid, userId, adminId)
}

// CreateAndAssignTask is a convenience method: creates a task and immediately assigns it.
func (a *TaskApp) CreateAndAssignTask(adminId, userId, title, desc string, days int, priority enum.Priority) string {
	tid := a.CreateTask(adminId, title, desc, days, priority)
	a.AssignTask(adminId, userId, tid)
	return tid
}

// CompleteNextTask picks the first queued task for userId and marks it done.
func (a *TaskApp) CompleteNextTask(userId string) {
	result := a.taskManager.DoTask(userId)
	fmt.Println("[App]:", result)
}

// DeleteTask removes a task from the system by ID.
func (a *TaskApp) DeleteTask(tid string) {
	a.taskManager.DeleteTask(tid)
}

// ─── History ─────────────────────────────────────────────────────────────────

// ShowHistory prints the full task history (assigned + completed) for a user.
func (a *TaskApp) ShowHistory(userId string) {
	fmt.Printf("\n[App]: Showing history for user %s\n", userId)
	a.taskManager.TasksHistory(userId)
}

// ─── Search ──────────────────────────────────────────────────────────────────

// SearchByPriority returns all tasks sorted by priority (HIGH → LOW).
func (a *TaskApp) SearchByPriority() {
	strat := search.NewSearchByPriority()
	tasks := strat.SearchTask(a.taskManager.GetAllTasks())
	fmt.Println("\n[Search] Tasks sorted by Priority (HIGH → LOW):")
	for _, t := range tasks {
		fmt.Printf("  ID=%-8s | Title=%-20s | Priority=%d\n",
			t.GetId(), t.GetTitle(), t.GetPriority())
	}
}

// SearchByDueDate returns all tasks sorted by due date (latest first).
func (a *TaskApp) SearchByDueDate() {
	strat := search.NewSearchByDueDate()
	tasks := strat.SearchTask(a.taskManager.GetAllTasks())
	fmt.Println("\n[Search] Tasks sorted by Due Date (latest → earliest):")
	for _, t := range tasks {
		fmt.Printf("  ID=%-8s | Title=%-20s | Due=%s\n",
			t.GetId(), t.GetTitle(), t.GetDueDate().Format("2006-01-02"))
	}
}