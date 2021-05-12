#pragma once
#include "sp_vm_api.h"
#include <functional>

class DebugListener : public SourcePawn::IDebugListener
{
public:
	std::function<void(const char*)> onError;
	SourcePawn::IDebugListener* oldListener;

	void OnDebugSpew(const char* msg, ...) override;
	void ReportError(const SourcePawn::IErrorReport &report, SourcePawn::IFrameIterator& iter) override;
	void OnContextExecuteError(SourcePawn::IPluginContext* ctx, SourcePawn::IContextTrace* error) override;
};

