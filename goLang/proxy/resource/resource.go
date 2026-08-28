package resource

type Resource interface {
	Do() string
}

type ActualResource struct {
}

func NewActualResource() ActualResource {
	return ActualResource{}
}
func (a ActualResource) Do() string {
	return "resoure called"
}

type Proxy struct {
	res Resource
}

func NewProxy(res Resource) Proxy {
	return Proxy{
		res,
	}
}

func (p Proxy) Do() string {
	return p.res.Do() + " via proxy"
}
