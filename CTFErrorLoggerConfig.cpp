#include "CTFErrorLoggerConfig.h"
#include <string>

using namespace SourceMod;
using namespace std;

SMCResult CTFErrorLoggerConfig::ReadSMC_KeyValue (const SMCStates *states, const char *key, const char *value)
{
    if (key && value)
    {
        if (strcmp(key, "sentry_dsn_url") == 0)
        {
            sentry_dsn_url = new string(value);
        }
        else if (strcmp(key, "server_name") == 0)
        {
            server_name = new string(value);
        }
        else if (strcmp(key, "server_id") == 0)
        {
            server_id = new string(value);
        }
        else if (strcmp(key, "environment") == 0)
        {
            environment = new string(value);
        }
        else if (strcmp (key, "region") == 0)
        {
            region = new string(value);
        }
    }

    return SMCResult::SMCResult_Continue;
}

CTFErrorLoggerConfig::~CTFErrorLoggerConfig ()
{
    delete sentry_dsn_url;
    delete server_name;
    delete server_id;
    delete environment;
}
