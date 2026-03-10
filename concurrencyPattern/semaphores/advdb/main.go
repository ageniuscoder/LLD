package main

import (
	"fmt"
	"strconv"
	"sync"
)

type Connection struct {
	Id int
}

func NewConnection(id int) *Connection {
	fmt.Println("New connection created, id: ",id)
	return &Connection{
		Id: id,
	}
}

func (c *Connection) Execute(query string) {
	fmt.Printf("Executed: conn Id %d , query: %s\n", c.Id, query)
}

type ConnectionPool struct {
	mu sync.Mutex
	idle chan *Connection
	maxConn int
	total int
}

func NewConnectionPool(num int) *ConnectionPool {
	return &ConnectionPool{
		idle: make(chan *Connection, num),
		maxConn: num,
	}
}

func (cp *ConnectionPool) Acquire() *Connection {
	select{
	case conn:=<-cp.idle:
		return conn
	default:
		fmt.Println("[select]: No idle connection")
	}

	cp.mu.Lock()

	if cp.total<cp.maxConn{
		cp.total++
		id:=cp.total
		cp.mu.Unlock()
		return NewConnection(id)
	}

	cp.mu.Unlock()
	return <-cp.idle
}

func (cp *ConnectionPool) Release(conn *Connection) {
	cp.idle <- conn
}

func main() {
	pool := NewConnectionPool(4)

	var wg sync.WaitGroup

	for i := 1; i <= 10; i++ {
		wg.Add(1)

		go func(tid int) {
			defer wg.Done()

			conn := pool.Acquire()
			defer pool.Release(conn)
			query := "query" + strconv.Itoa(tid)
			conn.Execute(query)
		}(i)
	}

	wg.Wait()
}