#include "DebugListener.h"
#include <sentry.h>

using namespace SourcePawn;

void DebugListener::OnDebugSpew(const char* msg, ...)
{
	if (oldListener)
	{
		oldListener->OnDebugSpew(msg);
	}
	onError(msg);
}

void DebugListener::ReportError(const IErrorReport& report, IFrameIterator& iter)
{
	if (oldListener)
	{
		oldListener->ReportError(report, iter);
	}
	onError(report.Message());


}

void DebugListener::OnContextExecuteError(IPluginContext* ctx, IContextTrace* error)
{
	//Not used
}
