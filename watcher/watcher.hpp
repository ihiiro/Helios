#pragma once
#include <iostream>
#include <windows.h>
#include <filesystem>

#define OBJECT_WAIT_TIME_MS 50
#define DEV_PATH_NUMBER 7
#define PROGRAM_FAILED 1
#define CHANGE_BUFFER_SIZE 1024

typedef struct {    
    HANDLE handle;
    OVERLAPPED overlapped;
    uint8_t changeBuf[CHANGE_BUFFER_SIZE];
    const char *watch_context;
    const char *backup_context;
} __CONTEXT;