#pragma once
#include <string>
#include "tier1/iconvar.h"
#include "convar.h"


/// <summary>
/// Configuration class for the CTF Error Logger. 
/// </summary>
class CTFErrorLoggerConfig
{
public:
    std::string sentry_dsn_url;
    ConVar* server_name;
    ConVar* server_id;
    ConVar* environment;
    ConVar* region;
    ConVar* logReaderWaitTime = 0;
    ConVar* type;

    CTFErrorLoggerConfig(ICvar* icvar);
};
