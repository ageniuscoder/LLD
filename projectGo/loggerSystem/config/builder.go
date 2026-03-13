package config

import (
	"fmt"
	"logger/appender"
	"logger/formatter"
	logs "logger/logger"
	"logger/logmsg"
)

func Build(cfg *LoggerConfig) (*logs.Logger,[]func(),error){
	var closers []func()
	level,ok:=logmsg.ParseLevel(cfg.MinLevel)
	if !ok{
		return nil,closers,fmt.Errorf("Config: not valid log levle: %q",level.ToStr())
	}
	system:=logs.GetInstance(cfg.Buffer,level)

	for _,lv:=range cfg.Levels{
		for _,ac:=range lv.Appenders{

			f,err:=buildFormatter(ac.Formatter)
			if err!=nil{
				return nil,closers,err
			}

			a,closer,err:=buildAppender(ac,f)
			if err!=nil{
				for _,c:=range closers{
					c()
				}
				return nil,nil,err
			}
			if closer!=nil{
				closers=append(closers, closer)
			}

			system.AddAppender(lv.Level,a)
		}
	}
	return system,closers,nil
}

func buildFormatter(fc formatterConfig) (formatter.LogFormatter,error){
	switch fc.Type{
	case "text":
		return formatter.NewTextFormatter(),nil
	case "json":
		return formatter.NewJsonFormatter(),nil
	default:
		return nil,fmt.Errorf("builder: unkonwn formatter type %q",fc.Type)
	}
}

func buildAppender(ac appenderConfig,f formatter.LogFormatter) (appender.LogAppender,func(),error){
	switch ac.Type{
	case "console":
		return appender.NewConsoleAppender(f),nil,nil
	case "file":
		fa,err:=appender.NewFileAppender(ac.Path,f)
		if err != nil {
			return nil, nil, fmt.Errorf("builder: cannot open log file %q: %w", ac.Path, err)
		}
		return fa,fa.CloseFile,nil
	default:
		return nil,nil,fmt.Errorf("builder: unkonwn appender type %q",ac.Type)
	}
}