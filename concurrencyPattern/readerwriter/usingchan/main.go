// rw lock using channel only,,,,core idea a manager is managing all the reader and writer req
package main
type RWlock struct{
	rlock chan chan struct{}
	runlock chan struct{}
	wlock chan chan struct{}
	wunlock chan struct{}
}

func NewRWlock() *RWlock{
	rw:= &RWlock{
		rlock: make(chan chan struct{}),
		runlock: make(chan struct{}),
		wlock: make(chan chan struct{}),
		wunlock: make(chan struct{}),
	}
	go rw.lockManager()
	return rw
}

func (rw *RWlock) lockManager(){
	readers:=0
	writer:=false

	waitingReaders:= []chan struct{}{}
	waitingWriters:=[]chan struct{}{}

	for{
		select{
		case reply:=<-rw.rlock:
			if !writer && len(waitingWriters)==0 {
				readers++
				reply<-struct{}{}
			}else{
				waitingReaders=append(waitingReaders,reply)
			}
		case <-rw.runlock:
			readers--
			if readers==0 && len(waitingWriters)>0 {
				writer=true
				w:=waitingWriters[0]
				waitingWriters=waitingWriters[1:]
				w<-struct{}{}
			}
		case reply:=<-rw.wlock:
			if !writer && readers==0 {
				writer=true
				reply<-struct{}{}
			}else{
				waitingWriters=append(waitingWriters, reply)
			}

		case <-rw.wunlock:
			writer=false

			if len(waitingReaders)>0 {
				for _,r:=range waitingReaders {
					readers++
					r<-struct{}{}
				}
				waitingReaders=nil
			}else if len(waitingWriters) > 0 {
				writer = true
				w := waitingWriters[0]
				waitingWriters = waitingWriters[1:]
				w <- struct{}{}
			}
			
		}
	}
}

func (rw *RWlock) Rlock(){
	reply:=make(chan struct{})
	rw.rlock<-reply
	<-reply
}

func (rw *RWlock) Runlock(){
	rw.runlock<-struct{}{}
}

func (rw *RWlock) Wlock(){
	reply:=make(chan struct{})
	rw.wlock<-reply
	<-reply
}

func (rw *RWlock) Wunlock(){
	rw.wunlock<-struct{}{}
}
func main(){}