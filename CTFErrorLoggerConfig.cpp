#include "CTFErrorLoggerConfig.h"

using namespace SourceMod;

SMCResult CTFErrorLoggerConfig::ReadSMC_KeyValue (const SMCStates *states, const char *key, const char *value)
{
    if (key && value)
    {
        if (key == "sentry_dsn_url")
        {
            sentry_dsn_url = value;
        }
        else if (key == "server_name")
        {
            server_name = value;
        }
        else if (key == "server_id")
        {
            server_id = value;
        }
        else if (key == "environment")
        {
            environment = value;
        }
    }

    return SMCResult::SMCResult_Continue;
}
