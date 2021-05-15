#pragma once
#include <ITextParsers.h>

class CTFErrorLoggerConfig : public SourceMod::ITextListener_SMC
{
public:
    const char* sentry_dsn_url;
    const char *server_name;
    const char *server_id;
    const char *environment;

    SourceMod::SMCResult ReadSMC_KeyValue (const SourceMod::SMCStates *states,
                                          const char *key,
                                          const char *value) override;
};
