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
            sentry_dsn_url = make_unique<string>(value);
        }
        else if (strcmp(key, "server_name") == 0)
        {
            server_name = make_unique<string>(value);
        }
        else if (strcmp(key, "server_id") == 0)
        {
            server_id = make_unique<string>(value);
        }
        else if (strcmp(key, "environment") == 0)
        {
            environment = make_unique<string>(value);
        }
        else if (strcmp (key, "region") == 0)
        {
            region = make_unique<string>(value);
        }
    }

    return SMCResult::SMCResult_Continue;
}
