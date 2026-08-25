package obs

type Channel interface {
	Add(u User)
	Notify()
	Remove(u User)
}

type striver struct {
	subs  []User
	video string
}

func NewStriver() *striver {
	return &striver{
		subs: []User{},
	}
}
func (s *striver) Add(u User) {
	s.subs = append(s.subs, u)
}
func (s *striver) Remove(u User) {
	for i, user := range s.subs {
		if u == user {
			s.subs = append(s.subs[:i], s.subs[i+1:]...)
			return
		}
	}
}

func (s *striver) Notify() {
	for _, sub := range s.subs {
		sub.Update()
	}
}

func (s *striver) UploadVideo(title string) {
	s.video = title
	s.Notify()
}

func (s *striver) FetchMsg() string {
	return s.video
}
