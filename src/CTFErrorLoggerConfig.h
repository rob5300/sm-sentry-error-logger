#pragma once
#include <string>
#include "tier1/iconvar.h"
#include "convar.h"


/// <summary>
/// Configuration class for the CTF Error Logger. 
/// Will assign expected data to the string pointers when parsing is done.
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

    CTFErrorLoggerConfig(ICvar* icvar);
};
