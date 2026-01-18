
#include <iostream>
#include <windows.h>
using namespace std;

#define DEV_PATH_NUMBER 2

#define SUCCESS 0
#define PROGRAM_FAILED 1

int main() 
{
    const char *path[DEV_PATH_NUMBER] = ["/c/Users/Administrator/Desktop/Helios/watcher/test0", "/c/Users/Administrator/Desktop/Helios/watcher/test1"];
    wchar_t wideCharPath[MAX_PATH];
    size_t mbstowcsReturn;
    HANDLE handles[DEV_PATH_NUMBER];

    cout << "creating change handles...\n";
    for ( int i = 0, j = 0; i < DEV_PATH_NUMBER; i++ ) 
    {
        if ( mbstowcs_s(&mbstowcsReturn, wideCharPath, MAX_PATH, path[i], MAX_PATH) != SUCCESS )
        {
            cerr << "mbstowcs_s() failed\n";
            return PROGRAM_FAILED;
        }
        if ( mbstowcsReturn == 0 )
        {
            cout << "empty watch-context path, ignored.\n";
            continue;
        }
        handle[j] = FindFirstChangeNotification((const char *)wideCharPath, TRUE, FILE_NOTIFY_CHANGE_SIZE);
        if ( handle[j] == INVALID_HANDLE_VALUE )
        {
            cout << "invalid handle, ignored.\n";
            continue;
        }
        j++;

    }

    HANDLE handle = FindFirstChangeNotification((const char *)wideCharPath, TRUE,
                                                FILE_NOTIFY_CHANGE_SIZE);

    if ( handle ==  INVALID_HANDLE_VALUE )
        return 1;

    const DWORD waitstatus = WaitForMultipleObjects(1, &handle, TRUE, 10000);

    if ( waitstatus == WAIT_OBJECT_0 )
        cout << "directory changed!";

    
}