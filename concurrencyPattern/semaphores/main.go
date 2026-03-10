package main //db connection pool with pre creation it is not lazy creation

import (
	"fmt"
	"strconv"
	"sync"
)

type Connection struct {
	Id int
}

func NewConnection(id int) *Connection {
	return &Connection{
		Id: id,
	}
}

func (c *Connection) Execute(query string) {
	fmt.Printf("Executed: conn Id %d , query: %s\n", c.Id, query)
}

type ConnectionPool struct {
	pool chan *Connection
}

func NewConnectionPool(num int) *ConnectionPool {
	p := ConnectionPool{
		pool: make(chan *Connection, num),
	}

	for i := 1; i <= num; i++ {
		p.pool <- NewConnection(i)
	}

	return &p
}

func (cp *ConnectionPool) Acquire() *Connection {
	return <-cp.pool
}

func (cp *ConnectionPool) Release(conn *Connection) {
	cp.pool <- conn
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