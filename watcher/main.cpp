
/*

The windows API documentation is terrible.
I learned how to use ReadDirectoryChangeW() from this guy: https://gist.github.com/nickav/a57009d4fcc3b527ed0f5c9cf30618f8


*/

#include "watcher.hpp"

int main() 
{
    rapidjson::Document doc;

    srand(time(0));
    ifstream slotsFile("../server/slots.json");
    string slotsString((istreambuf_iterator<char>(slotsFile)), 
                        istreambuf_iterator<char>() );
    doc.Parse(slotsString.c_str());
    if ( doc.HasParseError() )
        return cerr << "rapidjson parse error\n", PROGRAM_FAILED;

    vector<__CONTEXT> contexts(doc.MemberEnd() - doc.MemberBegin());

    cout << "INITIALIZING CONTEXTS...\n";
    int realNumberOfContexts = initializeContexts(doc, contexts);
    cout << "DONE.\n";

    cout << "WATCHER RUNNING...\n";
    startWatcherLoop(contexts, realNumberOfContexts);

   
    
}