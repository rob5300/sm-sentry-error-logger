#pragma once
#include <functional>
#include <IPluginSys.h>
#include <smsdk_ext.h>
#include <string>
#include <vector>
#include "sp_vm_api.h"
#include "SPSentryFrame.h"

class DebugListener : public SourcePawn::IDebugListener
{
public:
	std::function<void(const char*)> onError;
	SourcePawn::IDebugListener* oldListener;

	void OnDebugSpew(const char* msg, ...) override;
	void ReportError(const SourcePawn::IErrorReport &report, SourcePawn::IFrameIterator& iter) override;
	void OnContextExecuteError(SourcePawn::IPluginContext* ctx, SourcePawn::IContextTrace* error) override;

private:
	IPluginManager* pluginsys;

	std::vector<SPSentryFrame> DebugListener::GetStackTrace(IFrameIterator &iter);
};