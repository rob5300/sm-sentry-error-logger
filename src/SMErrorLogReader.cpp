#include "SMErrorLogReader.h"
#include <chrono>
#include <thread>
#include <fstream>

using namespace std;

#define ERROR_DATETIME_LEN 25

string ignoreStrings[] = {
    "SourceMod error session started",
    "[SM]",
    "Call stack trace",
    "Info (map",
    "Error log file session closed",
    "Exception reported"
};

SMErrorLogReader::SMErrorLogReader (string &_errorLogPath, int32_t& _waitTime)
{
	errorLogPath = _errorLogPath;
    waitTime = _waitTime;
    filesystem::path newestErrorLogPath = GetLatestErrorLogPath();
    ifstream errorLog(newestErrorLogPath);
    string line;
    //Populate all existing log file dates so these can be ignored as they are not "new"
    while (getline(errorLog, line)) {
        pastLogContents.insert(line.substr(0, ERROR_DATETIME_LEN));
    }

    active = true;
    thread = make_unique<std::thread>([this](){WatchErrorLog();});
}

void SMErrorLogReader::WatchErrorLog ()
{
    while (active)
    {
        filesystem::path newestErrorLogPath = GetLatestErrorLogPath();
        ifstream errorLog(newestErrorLogPath);
        string line;
        while (getline(errorLog, line)) {
            //Check if we have any of the date substrings
            string substr = line.substr(0, ERROR_DATETIME_LEN);
            string errorContents = line.substr(ERROR_DATETIME_LEN);
            if (!ContainsIgnoredStrings(errorContents) && pastLogContents.count(substr) == 0)
            {
                pastLogContents.insert(substr);
                printf((string("[SMErrorLogReader] New Error was found in the SM Error Log: '") + errorContents + string("'\n")).c_str());
                if(EventReciever != nullptr) EventReciever->OnSMErrorFound(errorContents);
            }
        }
        //const string message = string("[SMErrorLogReader] Error Log '") + newestErrorLogPath.filename().generic_string() + string("' was checked for new errors. Next try in ") + to_string(waitTime) + string(" seconds\n");
        //printf(message.c_str());

        if(!active) return;

        this_thread::sleep_for(chrono::seconds(waitTime));
    }
}

void SMErrorLogReader::Stop()
{
    active = false;
    thread->join();
}

filesystem::path SMErrorLogReader::GetLatestErrorLogPath()
{
    filesystem::path newestErrorLogPath;
    filesystem::file_time_type* lastModifyTime = nullptr;
    for (const auto &entry : filesystem::directory_iterator(errorLogPath))
    {
        if (entry.is_regular_file () && entry.path().filename().generic_string().find("error") != string::npos)
        {
            if (lastModifyTime == nullptr || entry.last_write_time() > *lastModifyTime)
            {
                lastModifyTime = &entry.last_write_time();
                newestErrorLogPath = entry.path();
            }
        }
    }
    return newestErrorLogPath;
}

bool SMErrorLogReader::ContainsIgnoredStrings(std::string& str)
{
    for (const auto &ignore : ignoreStrings)
    {
        if (str.find(ignore) != string::npos)
        {
            return true;
        }
    }
    return false;
}
