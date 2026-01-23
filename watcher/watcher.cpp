#include "watcher.hpp"

int initializeContexts(rapidjson::Document &doc, vector<__CONTEXT> &contexts)
{
    int i = 0;

    for (rapidjson::Value::ConstMemberIterator itr = doc.MemberBegin();
        itr != doc.MemberEnd(); ++itr)
    {
        const char *path = itr->value["watch_context"].GetString();
        cout << "path " << " [\"" << path << "\"]";
        contexts[i].backupContext = itr->value["backup_context"].GetString();
        contexts[i].watchContext = path;
        contexts[i].handle = CreateFile(
            path,
            FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
            NULL,
            OPEN_EXISTING,
            FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
            NULL
        );
        if (contexts[i].handle == INVALID_HANDLE_VALUE)
        {
            cerr << "\ninvalid handle, ignored.\n";
            continue;
        }
        contexts[i].overlapped.hEvent = CreateEvent(NULL, FALSE, 0, NULL);
        BOOL success = ReadDirectoryChangesW(
            contexts[i].handle, contexts[i].changeBuf, CHANGE_BUFFER_SIZE, TRUE,
            FILE_NOTIFY_CHANGE_FILE_NAME  | 
            FILE_NOTIFY_CHANGE_DIR_NAME   |
            FILE_NOTIFY_CHANGE_LAST_WRITE,
            NULL, &contexts[i].overlapped, NULL
        );
        if (!success)
        {
            cerr << "ReadDirectoryChangesW() failed\n";
            return PROGRAM_FAILED;
        }
        cout << " assimilated.\n";
        i++;
    }
    return i;
}

void startWatcherLoop(vector<__CONTEXT> contexts, int realNumberOfContexts)
{
     while (true)
        for (int i = 0; i < realNumberOfContexts; i++)
        {
            DWORD status = WaitForSingleObject(contexts[i].overlapped.hEvent, OBJECT_WAIT_TIME_MS);

            if (status == WAIT_OBJECT_0)
            {
                cout << "changes detected for [\"" << contexts[i].watchContext << "\"]" "\n";
                GetOverlappedResult(contexts[i].handle, 
                                    &contexts[i].overlapped,
                                    NULL,
                                    FALSE);
                FILE_NOTIFY_INFORMATION *event = (FILE_NOTIFY_INFORMATION*)contexts[i].changeBuf;
                BOOL success = ReadDirectoryChangesW(
                    contexts[i].handle, contexts[i].changeBuf, CHANGE_BUFFER_SIZE, TRUE,
                    FILE_NOTIFY_CHANGE_FILE_NAME  |
                    FILE_NOTIFY_CHANGE_DIR_NAME   |
                    FILE_NOTIFY_CHANGE_LAST_WRITE,
                    NULL, &contexts[i].overlapped, NULL
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

                    if (event->NextEntryOffset)
                        *((uint8_t**)&event) += event->NextEntryOffset;
                    else
                        break;
                }
                cout << "backing up...";
                filesystem::copy(contexts[i].watchContext, contexts[i].backupContext + 
                                            "/snapshot" + to_string(rand()) );
                cout << " done.\n";
            }
            else if (status == WAIT_FAILED)
                cerr << "WAIT_FAILED for [\"" << contexts[i].watchContext << "\"]" "\n"; 
        }
}