package core_test

import (
	"testing"

	"github.com/bagasjs/tako/core"
)

func TestSelect(t *testing.T) {
    res := core.Select("users", "id", "name").Where("email", "=", "foo@bar.com").ToString()
    t.Log(res)
}

func TestUpdate(t *testing.T) {
    res := core.Update("users").Set("age", 10).Where("email", "=", "foo@bar.com").ToString()
    t.Log(res)
}
