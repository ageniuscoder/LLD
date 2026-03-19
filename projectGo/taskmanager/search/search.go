package search

import (
	"sort"
	"taskmanager/task"
)

type SearchStrat interface {
	SearchTask(tasks map[string]*task.Task) []task.Task
}

type SearchByPriority struct{
}

func NewSearchByPriority() *SearchByPriority{
	return &SearchByPriority{}
}

func (sb *SearchByPriority) SearchTask(tasks map[string]*task.Task) []task.Task{     //maps in go are unordered
	t:=make([]task.Task,0)
	for _,val:=range tasks{
		t=append(t, *val)
	}
	sort.Slice(t,func(i,j int) bool{
		return t[i].GetPriority()>t[j].GetPriority()
	})
	return t
}

type SearchByDueDate struct{
}

func NewSearchByDueDate() *SearchByDueDate{
	return &SearchByDueDate{}
}

func (sd *SearchByDueDate) SearchTask(tasks map[string]*task.Task) []task.Task{
	t:=make([]task.Task,0)
	for _,val:=range tasks{
		t=append(t, *val)
	}
	sort.Slice(t,func(i,j int) bool{
		return t[i].GetDueDate().After(t[j].GetDueDate())
	})
	return t
}

