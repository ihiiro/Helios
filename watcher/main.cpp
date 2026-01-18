
#include <iostream>
#include <windows.h>
#include <filesystem>
using namespace std;


#define DEV_PATH_NUMBER 7

#define PROGRAM_FAILED 1

#define GAME_SAVING_WAIT_TIME 5000

int main() 
{
    const char *path[DEV_PATH_NUMBER] = {"", "", "", "\\Users\\Administrator\\Desktop\\cv\\", "", "/fdfdfd/", "/fdf"};
    wchar_t wideCharPath[MAX_PATH];
    size_t mbstowcsReturn;
    HANDLE handles[DEV_PATH_NUMBER];
    int realNumberOfHandles = 0;

    cout << "creating change handles...\n";
    for ( int i = 0, j = 0; i < DEV_PATH_NUMBER; i++ ) 
    {
        cout << "handle at index " << j << " [\"" << path[i] << "\"]";
        if ( !filesystem::exists(path[i]) )
        {
            cerr << "\npath " << path[i] << " does not exist, ignored\n";
            continue;
        }
        /* FindFirstChangeNotification() requires a wchar_t for some reason so this function fills wideCharPath from path[i] */
        if ( mbstowcs_s(&mbstowcsReturn, wideCharPath, MAX_PATH, path[i], MAX_PATH) != 0 )
        {
            cerr << "\nmbstowcs_s() failed\n";
            return PROGRAM_FAILED;
        }
        /* store the non-ignored change handles inside the array */
        handles[j] = FindFirstChangeNotification((const char *)wideCharPath, TRUE, FILE_NOTIFY_CHANGE_SIZE );
        if ( handles[j] == INVALID_HANDLE_VALUE )
        {
            cerr << "\ninvalid handle, ignored.\n";
            continue;
        }
        cout << " created.\n";
        j++;
        realNumberOfHandles++;

    }

    while ( true )
        for ( int i = 0; i < realNumberOfHandles; i++ )
        {
            DWORD status = WaitForSingleObject(handles[i], 0);

            if ( status == WAIT_OBJECT_0 )
            {
                cout << "write detected for handle at index " << i << "\n";
                if ( FindNextChangeNotification(handles[i]) == 0 )
                {
                    cerr << "batch FindNextChangeNotification() failed for handle at index " << i << "\n";
                    continue;
                }
                /* to avoid writing multiple incomplete backups, we "batch" signals instead */
                status = WaitForSingleObject(handles[i], GAME_SAVING_WAIT_TIME);
                // write backups code here
                if ( FindNextChangeNotification(handles[i]) == 0 )
                {
                    cerr << "bridge findNextChangeNotification() failed for handle at index " << i << "\n";
                    continue;
                }
                if ( status == WAIT_OBJECT_0 )
                    cout << "delayed writes detected for handle at index " << i << "\n";
                else if ( status == WAIT_FAILED )
                    cerr << "nested WAIT_FAILED for handle at index " << i << "\n";
            }
            else if ( status == WAIT_FAILED )
                cerr << "WAIT_FAILED for handle at index " << i << "\n";
        }
    
}