package state

type Context struct {
	s  State //curr state
	s1 State
	s2 State
	s3 State
	s4 State
}

func NewContext() *Context {
	s1 := NewS1()
	s2 := NewS2()
	s3 := NewS3()
	s4 := NewS4()
	return &Context{
		s:  s1,
		s1: s1,
		s2: s2,
		s3: s3,
		s4: s4,
	}
}
func (c *Context) moveState(s State) {
	c.s = s
}

func (c *Context) M1() {
	c.s.M1(c)

}

func (c *Context) M2() {
	c.s.M2()
	if c.s == c.s2 {
		c.moveState(c.s3)
	}
}

func (c *Context) M3() {
	c.s.M3()
	if c.s == c.s3 {
		c.moveState(c.s4)
	}
}

func (c *Context) M4() {
	c.s.M4()
	if c.s == c.s4 {
		c.moveState(c.s1)
	}
}
