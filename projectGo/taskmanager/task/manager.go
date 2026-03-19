package task

import (
	"fmt"
	"sync"
	enum "taskmanager/enums"
	"taskmanager/notification"
)

type TaskManager struct {
	Tasks   map[string]*Task          //tid,task
	AssignedHistory map[string][]*TaskHistory //uid,[]taskhistory
	CompletedHistory map[string][]*TaskHistory
	notification notification.Notifiler
	rwm sync.RWMutex
}

func NewTaskManager(notifier notification.Notifiler) *TaskManager {
	return &TaskManager{
		Tasks:   make(map[string]*Task),
		AssignedHistory: make(map[string][]*TaskHistory),
		CompletedHistory: make(map[string][]*TaskHistory) ,
		notification: notifier,
	}
}

// GetAllTasks returns a safe value-copy snapshot for read-only consumers (e.g. search).
// Callers never hold a reference to the live map, so no external lock is needed.
func (tm *TaskManager) GetAllTasks() map[string]*Task {
	tm.rwm.RLock()
	defer tm.rwm.RUnlock()
	snapshot := make(map[string]*Task, len(tm.Tasks))
	for k, v := range tm.Tasks {
		copied := *v
		snapshot[k] = &copied
	}
	return snapshot
}

func (tm *TaskManager) CreateTask(createdBy,title, description string, days int, pri enum.Priority) string{
	t:=NewTaskBuilder().WithTitle(title).WithDueDate(days).WithCreatedBy(createdBy).WithDescription(description).WithPriority(pri).Build()
	id:=t.GetId()
	tm.rwm.Lock()
	tm.Tasks[id]=&t
	tm.rwm.Unlock()
	return id
}

func (tm *TaskManager) DeleteTask(id string){
	tm.rwm.Lock()
	defer tm.rwm.Unlock()
	if _,ok:=tm.Tasks[id]; !ok{
		fmt.Printf("can,t delete,%s not exists",id)
		return
	}
	delete(tm.Tasks,id)
}

func (tm *TaskManager) notify(cid string,th *TaskHistory){
	switch th.GetActionType(){
	case enum.TODO:
		tm.notification.AssignUpdate(cid,th.GetUserId(),th.GetId())
	case enum.DONE:
		tm.notification.CompleteUpdate(cid,th.GetUserId(),th.GetId())
	}
	
}

func (tm *TaskManager) AssignTask(uid,tid string){
	tm.rwm.Lock()
	if _,ok:=tm.Tasks[tid]; !ok{
		tm.rwm.Unlock()
		fmt.Printf("[Assign Task]: %s doesn,t exist\n",tid)
		return
	}
	if tm.Tasks[tid].status==enum.PENDING{
		tm.Tasks[tid].ChangeStatus(enum.INPROGRESS)
		th:=NewTaskHistory(tid,uid,enum.TODO)
		tm.AssignedHistory[uid]=append(tm.AssignedHistory[uid],th)
		cid:=tm.Tasks[tid].GetCid()
		tm.rwm.Unlock()
		tm.notify(cid,th)
	}else{
		tm.rwm.Unlock()
		fmt.Printf("[Assign Task]: %s already completed or inprogress\n",tid)
	}
}

func (tm *TaskManager) PickTask(uid string) (string,error){
	tm.rwm.Lock()
	defer tm.rwm.Unlock()
	if len(tm.AssignedHistory[uid])==0{
		return "",fmt.Errorf("%s","No Task Available")
	}
	assigned:=tm.AssignedHistory[uid][0]
	tm.AssignedHistory[uid]=append(tm.AssignedHistory[uid][:0],tm.AssignedHistory[uid][1:]...)
	return assigned.GetId(),nil
}

func (tm *TaskManager) MarkCompleted(uid,tid string){
	th:=NewTaskHistory(tid,uid,enum.DONE)
	tm.rwm.Lock()
	tm.Tasks[tid].ChangeStatus(enum.COMPLETED)
	tm.CompletedHistory[uid]=append(tm.CompletedHistory[uid], th)
	cid:=tm.Tasks[tid].GetCid()
	tm.rwm.Unlock()
	tm.notify(cid,th)
}

func (tm *TaskManager) DoTask(uid string) string{
	tid,err:=tm.PickTask(uid)
	if err!=nil{
		return "Task Not Done"
	}
	tm.MarkCompleted(uid,tid)
	return fmt.Sprintf("%s id Completed",tid)
} 

func (tm *TaskManager) TasksHistory(uid string){
	tm.rwm.RLock()
	defer tm.rwm.RUnlock()
	for _,v:=range tm.AssignedHistory[uid]{
		fmt.Printf("TaskId: %s\n Status: %s\n",v.GetId(),v.GetActionType().ToStr())
	}
	for _,v:=range tm.CompletedHistory[uid]{
		fmt.Printf("TaskId: %s\n Status: %s\n",v.GetId(),v.GetActionType().ToStr())
	}
}