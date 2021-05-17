#pragma once
#include <ITextParsers.h>
#include <string>

class CTFErrorLoggerConfig : public SourceMod::ITextListener_SMC
{
public:
    std::string *sentry_dsn_url;
    std::string *server_name;
    std::string *server_id;
    std::string *environment;
    std::string *region;

    SourceMod::SMCResult ReadSMC_KeyValue (const SourceMod::SMCStates *states,
                                          const char *key,
                                          const char *value) override;

    ~CTFErrorLoggerConfig();
};
