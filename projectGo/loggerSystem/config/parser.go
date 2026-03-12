package config

import "sync"

type Parser interface {
	Parse(data []byte) (*LoggerConfig, error)
}

var (
	instance *ParserFactory
	once    sync.Once
)

type ParserFactory struct {
	parsers map[string]Parser
}

func GetInstance() *ParserFactory{
	once.Do(func() {
		instance=&ParserFactory{
			parsers: make(map[string]Parser),
		}
	})
	return instance
}

func (pf *ParserFactory) Register(ext string, p Parser) {
	if _, ok := pf.parsers[ext]; !ok {
		pf.parsers[ext] = p
	}
}

func (pf *ParserFactory) GetParser(ext string) (Parser, bool) {
	if p, ok := pf.parsers[ext]; ok {
		return p, true
	}
	return nil, false
}