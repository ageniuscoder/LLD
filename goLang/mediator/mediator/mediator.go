package mediator

type Mediator interface {
	SendTo(from, to, msg string)
	Broadcast(from, msg string)
	Register(Peer)
}

type ChatMediator struct {
	peers map[string]Peer
}

func NewChatMediator() *ChatMediator {
	return &ChatMediator{
		peers: make(map[string]Peer),
	}
}

func (c *ChatMediator) SendTo(from, to, msg string) {
	if peer, ok := c.peers[to]; ok {
		peer.recieve(from, msg)
	}
}

func (c *ChatMediator) Broadcast(from, msg string) {
	for _, peer := range c.peers {
		peer.recieve(from, msg)
	}
}

func (c *ChatMediator) Register(p Peer) {
	c.peers[p.(*User).id] = p
}
