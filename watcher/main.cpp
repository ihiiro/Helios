
#include "watcher.hpp"
using namespace std;

int main() 
{
    const char *path[DEV_PATH_NUMBER] = {"", "", "", "\\Users\\Administrator\\Desktop\\cv\\", "", "/fdfdfd/", "/fdf"};
    WATCH_CONTEXT context[DEV_PATH_NUMBER];
    int realNumberOfcontexts = 0;

    cout << "initializing monitoring tools...\n";
    for ( int i = 0, j = 0; i < DEV_PATH_NUMBER; i++ ) 
    {
        cout << "path " << " [\"" << path[i] << "\"]";
        context[j].path = path[i];
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
        BOOL success = ReadDirectoryChangesW(
            context[j].handle, context[j].changeBuf, CHANGE_BUFFER_SIZE, TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME  | 
            FILE_NOTIFY_CHANGE_DIR_NAME   |
            FILE_NOTIFY_CHANGE_LAST_WRITE,
            NULL, &context[j].overlapped, NULL
        );
        if ( !success )
        {
            cerr << "ReadDirectoryChangesW() failed\n";
            return PROGRAM_FAILED;
        }
        cout << " assimilated.\n";
        j++;
        realNumberOfcontexts++;

    }

    while ( true )
        for ( int i = 0; i < realNumberOfcontexts; i++ )
        {
            DWORD status = WaitForSingleObject(context[i].overlapped.hEvent, 500);

            if ( status == WAIT_OBJECT_0 )
            {
                cout << "changes detected for [\"" << context[i].path << "\"]" "\n";
                DWORD bytesTransferred;
                GetOverlappedResult(context[i].handle, 
                                    &context[i].overlapped,
                                    &bytesTransferred,
                                    FALSE); // possible TRUE to wait for pending operation
                FILE_NOTIFY_INFORMATION *event = (FILE_NOTIFY_INFORMATION*)context[i].changeBuf;
                while (true) 
                {
                    DWORD nameLen = event->FileNameLength / sizeof(wchar_t);

                    switch (event->Action)
                    {
                        case FILE_ACTION_ADDED:
                            wprintf(L"file added: %.*s\n", nameLen, event->FileName);
                            break;

                        case FILE_ACTION_REMOVED:
                            wprintf(L"file removed: %.*s\n", nameLen, event->FileName);
                            break;

                        case FILE_ACTION_MODIFIED:
                            wprintf(L"file modified: %.*s\n", nameLen, event->FileName);
                            break;

                        case FILE_ACTION_RENAMED_OLD_NAME:
                            wprintf(L"file renamed from: %.*s\n", nameLen, event->FileName);
                            break;

                        case FILE_ACTION_RENAMED_NEW_NAME:
                            wprintf(L" to %.*s\n", nameLen, event->FileName);
                            break;

                        default:
                            cout << "unknown action\n";
                    }

                    if ( event->NextEntryOffset )
                        *((uint8_t**)&event) += event->NextEntryOffset;
                    else
                        break;
                }
                // cout << "waiting for save to finish " << GAME_SAVING_WAIT_TIME << " seconds...\n";
                // sleep(GAME_SAVING_WAIT_TIME);
                // cout << "backing up...\n";
                //backup code here
            }
            else if ( status == WAIT_FAILED )
                cerr << "WAIT_FAILED for [\"" << context[i].path << "\"]" "\n";

            /* queue next change operation */
            BOOL success = ReadDirectoryChangesW(
                context[i].handle, context[i].changeBuf, CHANGE_BUFFER_SIZE, TRUE,
                FILE_NOTIFY_CHANGE_FILE_NAME  |
                FILE_NOTIFY_CHANGE_DIR_NAME   |
                FILE_NOTIFY_CHANGE_LAST_WRITE,
                NULL, &context[i].overlapped, NULL
            );
        }
    
}