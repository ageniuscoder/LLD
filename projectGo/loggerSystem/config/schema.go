package config

type LoggerConfig struct {
	Levels []levelConfig `json:"levels" validate:"required,min=1,dive"`
	Buffer int           `json:"buffer" validate:"gte=0,lte=100000"`
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