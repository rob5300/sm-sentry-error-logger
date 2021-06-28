#include "CTFErrorLoggerConfig.h"
#include <string.h>

using namespace SourceMod;
using namespace std;

SMCResult CTFErrorLoggerConfig::ReadSMC_KeyValue (const SMCStates *states, const char *key, const char *value)
{
    if (key && value)
    {
        if (strcmp(key, "sentry_dsn_url") == 0)
        {
            sentry_dsn_url = string(value);
        }
        else if (strcmp(key, "server_name") == 0)
        {
            server_name = string(value);
        }
        else if (strcmp(key, "server_id") == 0)
        {
            server_id = string(value);
        }
        else if (strcmp(key, "environment") == 0)
        {
            environment = string(value);
        }
        else if (strcmp (key, "region") == 0)
        {
            region = string(value);
        }
        else if (strcmp(key, "logReaderWaitTime") == 0)
        {
            logReaderWaitTime = stoi(value);
        }
    }

    return SMCResult::SMCResult_Continue;
}
