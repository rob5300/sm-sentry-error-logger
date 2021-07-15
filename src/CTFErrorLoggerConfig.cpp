#include "CTFErrorLoggerConfig.h"

CTFErrorLoggerConfig::CTFErrorLoggerConfig(ICvar* icvar)
{
    server_id = icvar->FindVar("ce_server_index")->GetString();
    sentry_dsn_url = icvar->FindVar("ce_sentry_dsn_url")->GetString();
    server_name = icvar->FindVar("ce_server_name")->GetString();

    if (server_name.length() < 1)
    {
        const ConVar* hostname = icvar->FindVar("hostname");
        if (hostname != nullptr)
        {
            server_name = hostname->GetString();
        }
    }

    environment = icvar->FindVar("ce_environment")->GetString();
    region = icvar->FindVar("ce_region")->GetString();
    logReaderWaitTime = icvar->FindVar("ce_logreaderwaittime")->GetInt();
}
