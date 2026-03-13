package config

import (
	"encoding/json"
	"fmt"
)

type JsonParser struct {
}

func (jp *JsonParser) Parse(data []byte) (*LoggerConfig, error) {
	cfg := &LoggerConfig{}
	if err := json.Unmarshal(data, cfg); err != nil {
		return nil, fmt.Errorf("json parser: %w", err)
	}
	return cfg, nil
}

func init(){   //this is for self registry
	GetInstance().Register("json",&JsonParser{})
}