package config

type LoggerConfig struct {
	Levels        []levelConfig `json:"levels" validate:"required,min=1,dive"`
	Buffer        int           `json:"buffer" validate:"gte=0,lte=100000"`
	MinLevel      string        `json:"min_level" validate:"oneof=debug info warning error"`
	BatchSize     int           `json:"batch_size" validate:"gte=1,lte=1000"`
	FlushInterval int           `json:"flush_interval" validate:"gte=10,lte=900"` //in milisecond
}

type formatterConfig struct {
	Name string `json:"name" validate:"required"`
	Type string `json:"type" validate:"required,oneof=json text"`
}

type appenderConfig struct {
	Name      string          `json:"name" validate:"required"`
	Type      string          `json:"type" validate:"required,oneof=console file"`
	Formatter formatterConfig `json:"formatter" validate:"required"`
	Path      string          `json:"path,omitempty" validate:"required_if=Type file"`
}

type levelConfig struct {
	Level     string           `json:"level" validate:"required,oneof=debug info warning error"`
	Appenders []appenderConfig `json:"appenders" validate:"required,min=1,dive"`
}