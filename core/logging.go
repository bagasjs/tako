package core 

import (
	"fmt"
	"os"
)

type LogLevel string

func Info(args ...any) {
    fmt.Fprint(os.Stdout, "[INFO]")
    fmt.Fprintln(os.Stdout, args...)
}

func Warn(args ...any) {
    fmt.Fprint(os.Stdout, "[WARN]")
    fmt.Fprintln(os.Stdout, args...)
}

func Fatal(args ...any) {
    panic(fmt.Sprintln("[FATAL]", fmt.Sprint(args...)))
}

