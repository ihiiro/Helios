
#include "watcher.hpp"
using namespace std;

int main() 
{
    const char *path[DEV_PATH_NUMBER] = {"", "", "", "\\Users\\Administrator\\Desktop\\cv\\", "", "/fdfdfd/", "/fdf"};
    wchar_t wideCharPath[MAX_PATH];
    size_t mbstowcsReturn;
    WATCH_CONTEXT context[DEV_PATH_NUMBER];
    int realNumberOfcontext = 0;

    cout << "initializing monitoring tools...\n";
    for ( int i = 0, j = 0; i < DEV_PATH_NUMBER; i++ ) 
    {
        cout << "path " << " [\"" << path[i] << "\"]";
        if ( !filesystem::exists(path[i]) )
        {
            cerr << " does not exist, ignored\n";
            continue;
        }
        /* FindFirstChangeNotification() requires a wchar_t for some reason so this function fills wideCharPath from path[i] */
        if ( mbstowcs_s(&mbstowcsReturn, wideCharPath, MAX_PATH, path[i], MAX_PATH) != 0 )
        {
            cerr << "\nmbstowcs_s() failed\n";
            return PROGRAM_FAILED;
        }
        context[j].handle = CreateFile(
            path[i],
            FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
            NULL
        );
        if ( context[j].handle == INVALID_HANDLE_VALUE )
        {
            cerr << "\ninvalid handle, ignored.\n";
            continue;
        }
        /* this object is used to supply the event handle to ReadDirectoryChangesW() 
        which we'll later wait on for signals */
        context[j].overlapped.hEvent = CreateEvent(NULL, FALSE, 0, NULL);
        /* the change buffer is where change information is stored
        (we do that ourselves via another windows function ) */
        // uint8_t changeBuf[CHANGE_BUFFER_SIZE];
        BOOL success = ReadDirectoryChangesW(
            context[j].handle, context[j].changeBuf, CHANGE_BUFFER_SIZE, TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME  | 
            FILE_NOTIFY_CHANGE_DIR_NAME   |
            FILE_NOTIFY_CHANGE_LAST_WRITE,
            NULL, &context[j].overlapped, NULL
        );
        cout << " assimilated.\n";
        j++;
        realNumberOfcontext++;

    }


    // while ( true )
    //     for ( int i = 0; i < realNumberOfcontext; i++ )
    //     {
    //         DWORD status = WaitForSingleObject(context[i], 0);

    //         if ( status == WAIT_OBJECT_0 )
    //         {
    //             cout << "write detected for handle at index " << i << "\n";
    //             if ( FindNextChangeNotification(context[i]) == 0 )
    //             {
    //                 cerr << "batch FindNextChangeNotification() failed for handle at index " << i << "\n";
    //                 continue;
    //             }
    //             /* to avoid writing multiple incomplete backups, we "batch" signals instead */
    //             status = WaitForSingleObject(context[i], GAME_SAVING_WAIT_TIME);
    //             // write backups code here
    //             if ( FindNextChangeNotification(context[i]) == 0 )
    //             {
    //                 cerr << "bridge findNextChangeNotification() failed for handle at index " << i << "\n";
    //                 continue;
    //             }
    //             if ( status == WAIT_OBJECT_0 )
    //                 cout << "delayed writes detected for handle at index " << i << "\n";
    //             else if ( status == WAIT_FAILED )
    //                 cerr << "nested WAIT_FAILED for handle at index " << i << "\n";
    //         }
    //         else if ( status == WAIT_FAILED )
    //             cerr << "WAIT_FAILED for handle at index " << i << "\n";
    //     }
    
}