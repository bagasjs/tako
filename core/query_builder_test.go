package core_test

import (
	"testing"

	"github.com/bagasjs/tako/core"
)

func TestQueryBuilder(t *testing.T) {
    t.Log(core.SelectFrom("users", "name", "values", "test").Where("name", "=", "Bagas").ToString())
    t.Log(core.UpdateTable("users").Set("age", 10).Where("name", "=", "Bagas").ToString())
}
