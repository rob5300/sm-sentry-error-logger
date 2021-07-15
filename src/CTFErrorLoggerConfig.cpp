#include "CTFErrorLoggerConfig.h"

CTFErrorLoggerConfig::CTFErrorLoggerConfig(ICvar* icvar)
{
    server_id = icvar->FindVar("ce_server_index")->GetString();
    sentry_dsn_url = icvar->FindVar("ce_sentry_dsn_url")->GetString();
    server_name = icvar->FindVar("ce_server_name")->GetString();
    environment = icvar->FindVar("ce_environment")->GetString();
    region = icvar->FindVar("ce_region")->GetString();
    logReaderWaitTime = icvar->FindVar("ce_logreaderwaittime")->GetInt();
}
