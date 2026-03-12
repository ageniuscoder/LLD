package main

import "logger/config"

func main() {
	cfg, err := config.Load("./logger.json")
	if err != nil {
		panic(err)
	}

	system,closers,err:=config.Build(cfg)
	if err != nil {
		panic(err)
	}

	for _,c:=range closers{
		defer c()
	}

	defer system.Shutdown()

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