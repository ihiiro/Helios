#pragma once
#include <iostream>
#include <windows.h>
#include <filesystem>

#define DEV_PATH_NUMBER 7
#define PROGRAM_FAILED 1
#define GAME_SAVING_WAIT_TIME 5
#define CHANGE_BUFFER_SIZE 1024

typedef struct {    
    HANDLE handle;
    OVERLAPPED overlapped;
    uint8_t changeBuf[CHANGE_BUFFER_SIZE];
    const char *path;
} WATCH_CONTEXT;