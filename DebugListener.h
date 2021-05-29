#pragma once
#include <functional>
#include <IPluginSys.h>
#include <smsdk_ext.h>
#include <string>
#include <vector>
#ifdef WIN
#define SENTRY_BUILD_STATIC
#endif
#include "sentry.h"
#include "sp_vm_api.h"
#include "SPSentryFrame.h"
#include "CTFErrorLoggerConfig.h"

class DebugListener : public SourcePawn::IDebugListener
{
public:
	std::function<void(const char*)> onError;
	SourcePawn::IDebugListener* oldListener;
    CTFErrorLoggerConfig *config;

	void OnDebugSpew(const char* msg, ...) override;
	void ReportError(const SourcePawn::IErrorReport &report, SourcePawn::IFrameIterator& iter) override;
	void OnContextExecuteError(SourcePawn::IPluginContext* ctx, SourcePawn::IContextTrace* error) override;
    sentry_value_t DebugListener::GetBaseMessage (const char *blame, const char *message);

private:
	std::vector<SPSentryFrame> DebugListener::GetStackTrace(IFrameIterator &iter);
    
};