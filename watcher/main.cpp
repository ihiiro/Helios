
/*

The windows API documentation is terrible.
I learned how to use ReadDirectoryChangeW() from this guy: https://gist.github.com/nickav/a57009d4fcc3b527ed0f5c9cf30618f8


*/

#include "watcher.hpp"
using namespace std;

int main() 
{
    srand(time(0));
    rapidjson::Document doc;
    ifstream slotsFile("../server/slots.json");
    string slotsString((istreambuf_iterator<char>(slotsFile)), 
                        istreambuf_iterator<char>() );
    doc.Parse(slotsString.c_str());
    if ( doc.HasParseError() )
        return cerr << "rapidjson parse error\n", PROGRAM_FAILED;
    vector<__CONTEXT> context(doc.MemberEnd() - doc.MemberBegin());


    int realNumberOfcontexts = 0;
    int i = 0;

    cout << "initializing monitoring tools...\n";
    for ( rapidjson::Value::ConstMemberIterator itr = doc.MemberBegin();
        itr != doc.MemberEnd(); ++itr )
    {

        const char *path = itr->value["watch_context"].GetString();
        cout << "path " << " [\"" << path << "\"]";
        context[i].snapshotIndex = 0;
        context[i].backupContext = itr->value["backup_context"].GetString();
        context[i].watchContext = path;
        context[i].handle = CreateFile(
            path,
            FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
            NULL
        );
        if ( context[i].handle == INVALID_HANDLE_VALUE )
        {
            cerr << "\ninvalid handle, ignored.\n";
            continue;
        }
        /* this object is used to supply the event handle to ReadDirectoryChangesW() 
        which we'll later wait on for signals */
        context[i].overlapped.hEvent = CreateEvent(NULL, FALSE, 0, NULL);
        BOOL success = ReadDirectoryChangesW(
            context[i].handle, context[i].changeBuf, CHANGE_BUFFER_SIZE, TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME  | 
            FILE_NOTIFY_CHANGE_DIR_NAME   |
            FILE_NOTIFY_CHANGE_LAST_WRITE,
            NULL, &context[i].overlapped, NULL
        );
        if ( !success )
        {
            cerr << "ReadDirectoryChangesW() failed\n";
            return PROGRAM_FAILED;
        }
        cout << " assimilated.\n";
        i++;
        realNumberOfcontexts++;


    }

    while ( true )
        for ( int i = 0; i < realNumberOfcontexts; i++ )
        {
            DWORD status = WaitForSingleObject(context[i].overlapped.hEvent, OBJECT_WAIT_TIME_MS);

            if ( status == WAIT_OBJECT_0 )
            {
                cout << "changes detected for [\"" << context[i].watchContext << "\"]" "\n";
                DWORD bytesTransferred;
                GetOverlappedResult(context[i].handle, 
                                    &context[i].overlapped,
                                    &bytesTransferred,
                                    FALSE); // possible TRUE to wait for pending operation
                FILE_NOTIFY_INFORMATION *event = (FILE_NOTIFY_INFORMATION*)context[i].changeBuf;
                /* queue next changes */
                BOOL success = ReadDirectoryChangesW(
                    context[i].handle, context[i].changeBuf, CHANGE_BUFFER_SIZE, TRUE,
                    FILE_NOTIFY_CHANGE_FILE_NAME  |
                    FILE_NOTIFY_CHANGE_DIR_NAME   |
                    FILE_NOTIFY_CHANGE_LAST_WRITE,
                    NULL, &context[i].overlapped, NULL
                );
                while (true) 
                {
                    DWORD nameLen = event->FileNameLength / sizeof(wchar_t);

                    switch (event->Action)
                    {
                        case FILE_ACTION_ADDED:
                            wprintf(L"file added: %.*ls\n", nameLen, event->FileName);
                            break;

                        case FILE_ACTION_REMOVED:
                            wprintf(L"file removed: %.*ls\n", nameLen, event->FileName);
                            break;

                        case FILE_ACTION_MODIFIED:
                            wprintf(L"file modified: %.*ls\n", nameLen, event->FileName);
                            break;

                        case FILE_ACTION_RENAMED_OLD_NAME:
                            wprintf(L"file renamed from: %.*ls\n", nameLen, event->FileName);
                            break;

                        case FILE_ACTION_RENAMED_NEW_NAME:
                            wprintf(L" to %.*ls\n", nameLen, event->FileName);
                            break;

                        default:
                            cout << "unknown action\n";
                    }

                    if ( event->NextEntryOffset )
                        *((uint8_t**)&event) += event->NextEntryOffset;
                    else
                        break;
                }
                cout << "backing up...";
                filesystem::copy(context[i].watchContext, context[i].backupContext + 
                                            "/snapshot" + to_string(rand()) );
                cout << " done.\n";
                context[i].snapshotIndex++;
            }
            else if ( status == WAIT_FAILED )
                cerr << "WAIT_FAILED for [\"" << context[i].watchContext << "\"]" "\n";

            
        }
    
}