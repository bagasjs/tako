package core

func Pick(condition bool, a interface{}, b interface{}) interface{} {
    if condition {
        return a
    } else {
        return b
    }
}
