#pragma once
#include <functional>
#include <IPluginSys.h>
#include <smsdk_ext.h>
#include <string>
#include <vector>
#include <memory>
#ifdef _WIN32
#define SENTRY_BUILD_STATIC
#endif
#include "lib/sentry.h"
#include "sp_vm_api.h"
#include "SPSentryFrame.h"
#include "CTFErrorLoggerConfig.h"
#include "SMErrorLogReader.h"
#include <IGameHelpers.h>

/// <summary>
/// Custom debug listener to forward source pawn errors to the sentry api.
/// </summary>
class DebugListener : public SourcePawn::IDebugListener, public IErrorLogEventReciever
{
public:
	//Generic 
	std::function<void()> onError;
	SourcePawn::IDebugListener* oldListener;
    std::shared_ptr<CTFErrorLoggerConfig> config;

	/// <summary>
	/// Called by SM for a generic error with a message.
	/// </summary>
	/// <param name="msg">Potential message from the debug spew.</param>
	void OnDebugSpew(const char* msg, ...) override;

	/// <summary>
	/// Called by SM when a plugin throws an error/exception.
	/// </summary>
	void ReportError(const SourcePawn::IErrorReport &report, SourcePawn::IFrameIterator& iter) override;

	/// <summary>
	/// Depricated but implemented for safety.
	/// </summary>
	void OnContextExecuteError(SourcePawn::IPluginContext* ctx, SourcePawn::IContextTrace* error) override;

	/// <summary>
	/// Construct a base message sentry event and add common message data.
	/// </summary>
	/// <param name="blame">Error source</param>
	/// <param name="message">Main error message</param>
	/// <returns>New Sentry event object</returns>
    sentry_value_t GetBaseMessage (const char *blame, const char *message);

	/// <summary>
	/// Get notified when a new Log File error is found.
	/// </summary>
	/// <param name="error">Raw error string, with date removed.</param>
	void OnSMErrorFound (std::string& error);

private:
	/// <summary>
	/// Get the error stack information for sentry from the error. 
	/// Modified version of SourceMod DebugReporter::GetStackTrace
	/// </summary>
	std::vector<SPSentryFrame> GetStackTrace(IFrameIterator &iter);
    
};