package task

import enum "taskmanager/enums"

type TaskHistory struct {
	tid       string
	belongsTo string
	action    enum.Action
}

func NewTaskHistory(tid, uid string,act enum.Action) *TaskHistory {
	return &TaskHistory{
		tid:       tid,
		belongsTo: uid,
		action: act,
	}
}

func (th *TaskHistory) GetUserId() string {
	return th.belongsTo
}

func (th *TaskHistory) GetActionType() enum.Action{
	return th.action
}

func (th *TaskHistory) GetId() string{
	return th.tid
}