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
    std::unique_ptr<std::string> sentry_dsn_url;
    std::unique_ptr<std::string> server_name;
    std::unique_ptr<std::string> server_id;
    std::unique_ptr<std::string> environment;
    std::unique_ptr<std::string> region;

    /// <summary>
    /// Method to recieve parsed key value pairs.
    /// </summary>
    SourceMod::SMCResult ReadSMC_KeyValue (const SourceMod::SMCStates *states,
                                          const char *key,
                                          const char *value) override;
};
