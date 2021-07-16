#include "CTFErrorLoggerConfig.h"

CTFErrorLoggerConfig::CTFErrorLoggerConfig(ICvar* icvar)
{
    server_id = icvar->FindVar("ce_server_index");
    sentry_dsn_url = icvar->FindVar("ce_sentry_dsn_url")->GetString();
    server_name = icvar->FindVar("hostname");
    environment = icvar->FindVar("ce_environment");
    region = icvar->FindVar("ce_region");
    logReaderWaitTime = icvar->FindVar("ce_logreaderwaittime");
}
