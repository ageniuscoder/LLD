package config

import (
	"fmt"
	"os"
	"path/filepath"
	"strings"
	"sync"

	"github.com/go-playground/validator/v10"
)
var (
    validate     *validator.Validate
    validateOnce sync.Once
)

func getValidator() *validator.Validate {
    validateOnce.Do(func() { validate = validator.New() })
    return validate
}
func validateStruct(cfg *LoggerConfig) error {
	validate=getValidator()
	err := validate.Struct(cfg)
	if err == nil {
		return nil
	}

	var msg strings.Builder

	for _, e := range err.(validator.ValidationErrors) {

		msg.WriteString(
			fmt.Sprintf(
				"config validation error: field '%s' failed on '%s'",
				e.StructNamespace(),
				e.Tag(),
			),
		)

		if e.Param() != "" {
			msg.WriteString(fmt.Sprintf(" (%s)", e.Param()))
		}

		msg.WriteString("\n")
	}

	return fmt.Errorf("%s",msg.String())
}

func Load(path string) (*LoggerConfig, error) {
	data,err:=os.ReadFile(path)
	if err!=nil{
		return nil,fmt.Errorf("config: can,t read file %q: %w",path,err)
	}
	ext:=strings.ToLower(filepath.Ext(path))
	ext=ext[1:]
	jf:=GetInstance()
	parser,ok:=jf.GetParser(ext)
	if !ok{
		return nil,fmt.Errorf("parser not exists for ext %q",ext)
	}
	cfg,err:=parser.Parse(data)
	if err!=nil{
		return nil,fmt.Errorf("parser error: %w",err)
	}
	if err:=validateStruct(cfg); err!=nil{
		return nil,fmt.Errorf("validation error: %w",err)
	}
	return cfg,nil
}