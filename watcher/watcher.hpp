#pragma once
#include <iostream>
#include <windows.h>
#include <filesystem>
#include <fstream>
#include <vector>

#include "./rapidjson/include/rapidjson/document.h"

#define OBJECT_WAIT_TIME_MS 50
#define PROGRAM_FAILED 1
#define CHANGE_BUFFER_SIZE 1024

typedef struct {    
    HANDLE handle;
    OVERLAPPED overlapped;
    uint8_t changeBuf[CHANGE_BUFFER_SIZE];
    const char *watchContext;
    std::string backupContext;
} __CONTEXT;