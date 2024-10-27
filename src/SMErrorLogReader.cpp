#include "SMErrorLogReader.h"
#include <chrono>
#include <thread>
#include <fstream>
#include "smsdk_config.h"

using namespace std;

#define ERROR_DATETIME_LEN 25

///Strings to ignore. Errors with these are ignored.
string ignoreStrings[] = {
    "SourceMod error session started",
    "[SM]",
    "Call stack trace",
    "Info (map",
    "Error log file session closed",
    "Exception reported"
};

SMErrorLogReader::SMErrorLogReader (string &_errorLogPath, ConVar* _waitTime, ConVar* silent)
{
	errorLogPath = _errorLogPath;
    waitTime = _waitTime;
    this->silent = silent;
    filesystem::path newestErrorLogPath = GetLatestErrorLogPath();
    ifstream errorLog(newestErrorLogPath);
    string line;
    //Populate all existing log file dates so these can be ignored as they are not "new"
    while (getline(errorLog, line))
    {
        if (line.length() > ERROR_DATETIME_LEN)
        {
            pastLogContents.insert(line.substr(0, ERROR_DATETIME_LEN));
        }
    }

    if (!silent->GetBool())
    {
        printf("[%s] Found %u lines already in error log to be ignored.\n", SMEXT_CONF_NAME, pastLogContents.size());
    }

    active = true;
    //Start a new thread which will re check the error log for new errors.
    thread = make_unique<std::thread>([this](){WatchErrorLog();});
}

void SMErrorLogReader::WatchErrorLog ()
{
    while (active)
    {
        filesystem::path newestErrorLogPath = GetLatestErrorLogPath();
        ifstream errorLog(newestErrorLogPath);
        string line;
        while (getline(errorLog, line))
        {
            if(line.length() > ERROR_DATETIME_LEN){
                //Get the date+time component
                string dateTimeSubStr = line.substr(0, ERROR_DATETIME_LEN);
                //Get the error message component
                string errorContents = line.substr(ERROR_DATETIME_LEN);

                //Log this error if this date+time was not already seen, and if the error doesnt have the ignored strings in it.
                if (!ContainsIgnoredStrings(errorContents) && pastLogContents.count(dateTimeSubStr) == 0)
                {
                    pastLogContents.insert(dateTimeSubStr);

                    if (!silent->GetBool())
                    {
                         printf("[%s] New Error was found in the SM Error Log: '%s'.\n", SMEXT_CONF_NAME, errorContents.c_str());
                    }

                    if(EventReciever != nullptr) EventReciever->OnSMErrorFound(errorContents);
                }
            }
        }

        if (!silent->GetBool())
        {
            printf("[%s] Error Log '%s' was checked for new errors. Next try in %.0f seconds.\n'", SMEXT_CONF_NAME, newestErrorLogPath.filename().string().c_str(), waitTime->GetFloat());
        }

        if(!active) return;

        //Wait for the specified time to check the logs again.
        std::unique_lock<std::mutex> lk(loopMutex);
        loopConditionVar.wait_for(lk, chrono::seconds(waitTime->GetInt()), [this]{return !active;});
        lk.unlock();
        loopConditionVar.notify_one();
    }
}

void SMErrorLogReader::Stop()
{
    //Trigger the condition variable to stop waiting early so we can join and stop the thread now.
    printf("Attempting to stop WatchErrorLog thread...\n");
    std::unique_lock<std::mutex> lk(loopMutex);
    active = false;
    loopConditionVar.notify_one();
    lk.unlock();
    thread->join();
    printf("Stopped WatchErrorLog thread.\n");
}

filesystem::path SMErrorLogReader::GetLatestErrorLogPath()
{
    filesystem::path newestErrorLogPath;
    filesystem::file_time_type* lastModifyTime = nullptr;
    for (const auto &entry : filesystem::directory_iterator(errorLogPath))
    {
        //Make sure this is a file + the filename has 'error' in it.
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

bool SMErrorLogReader::ContainsIgnoredStrings(const std::string& str)
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
