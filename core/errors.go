package core

type Error struct {
    Message string `json:"message"`
    Code int `json:"code"`
}

func NewError(code int, message string) *Error {
    return &Error {
        Code: code,
        Message: message,
    }
}
