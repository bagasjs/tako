package core

type errorImpl struct {
    Message string `json:"message"`
    Code int `json:"code"`
}


type Error *errorImpl

func NewError(message string) Error {
    return &errorImpl {
        Message: message,
        Code: 0,
    }
}

func NewErrorWithCode(code int, message string) Error {
    return &errorImpl {
        Code: code,
        Message: message,
    }
}
