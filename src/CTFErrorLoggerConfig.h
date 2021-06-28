#pragma once
#include <ITextParsers.h>
#include <string>
#include <memory>


/// <summary>
/// Configuration class for the CTF Error Logger. 
/// Will assign expected data to the string pointers when parsing is done.
/// </summary>
class CTFErrorLoggerConfig : public SourceMod::ITextListener_SMC
{
public:
    std::string sentry_dsn_url;
    std::string server_name;
    std::string server_id;
    std::string environment;
    std::string region;
    int32_t logReaderWaitTime;

    /// <summary>
    /// Method to recieve parsed key value pairs.
    /// </summary>
    SourceMod::SMCResult ReadSMC_KeyValue (const SourceMod::SMCStates *states,
                                          const char *key,
                                          const char *value) override;
};
