package main

import (
	"logger/appender"
	"logger/formatter"
	logs "logger/logger"
)

func main() {
	tfor := formatter.NewTextFormatter()
	jfor:=formatter.NewJsonFormatter()
	capp:=appender.NewConsoleAppender(jfor)
	fapp,err:=appender.NewFileAppender("./logs/test.log",tfor)
	if err!=nil{
		panic(err)
	}
	defer fapp.CloseFile()

	system:=logs.GetInstance()
	defer system.Shutdown()
	system.AddAppender("debug",capp)
	system.AddAppender("info",capp)
	system.AddAppender("warning",capp)
	system.AddAppender("warning",fapp)
	system.AddAppender("error",capp)
	system.AddAppender("error",fapp)

	// ---- TEST LOGGING ----

	system.Debug("Debug message: application starting")

	system.Info("Info message: server initialized")

	system.Warning("Warning message: memory usage high")

	system.Error("Error message: database connection failed")

	system.Debug("Debug message: processing request")

	system.Info("Info message: request completed")

	system.Warning("Warning message: cache miss")

	system.Error("Error message: unable to write to disk")

}