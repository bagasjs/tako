package cmd

import "os"

type ProjectConfig struct {
    Name string 
    PublicDirectoryPermission os.FileMode
    PrivateDirectoryPermission os.FileMode
    PublicFilePermission os.FileMode
    PrivateFilePermission os.FileMode
}

var projectDirectories = []string {
    "./app/entity",
    "./app/model",
    "./app/repository",
    "./app/service",
    "./cmd",
    "./http/controllers",
    "./http/middlewares",
    "./migrations/sqlite3",
    "./res/views",
    "./res/static",
}

func CreateProject(info *ProjectConfig) {
    for _, dir := range projectDirectories {
        os.MkdirAll(dir, info.PublicDirectoryPermission)
    }
}

func DefaultProjectConfig(name string) *ProjectConfig {
    var config = &ProjectConfig{}
    config.Name = name
    config.PublicDirectoryPermission = 0755
    config.PublicFilePermission = 0644
    config.PrivateDirectoryPermission = 0700
    config.PrivateFilePermission = 0600
    return config
}

