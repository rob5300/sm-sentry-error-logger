#include "DebugListener.h"
#define SENTRY_BUILD_STATIC 1
#include <sentry.h>

using namespace SourcePawn;
using namespace std;

#define sentry_setstrvalue(object, strkey, strvalue) sentry_value_set_by_key(object, strkey, sentry_value_new_string(strvalue));
#define sentry_setintvalue(object, strkey, intvalue) sentry_value_set_by_key(object, strkey, sentry_value_new_int32(intvalue));

void DebugListener::OnDebugSpew(const char* msg, ...)
{
	if (oldListener)
	{
		oldListener->OnDebugSpew(msg);
	}
	onError(msg);
	const sentry_value_t event = sentry_value_new_message_event(SENTRY_LEVEL_DEBUG, "Debug", msg);
	sentry_capture_event(event);
}

void DebugListener::ReportError(const IErrorReport& report, IFrameIterator& iter)
{
	if (oldListener)
	{
		oldListener->ReportError(report, iter);
	}
	//Skip unrunnable functions as sm does.
	if (report.Code() == SP_ERROR_NOT_RUNNABLE)
		return;

	onError(report.Message());

	const char* blame = nullptr;
	try{
		//Get plugin name as sm does.
		if (report.Blame())
		{
			blame = report.Blame()->DebugName();
		}
		else
		{
			// Find the nearest plugin to blame.
			for (; !iter.Done(); iter.Next())
			{
				if (iter.IsScriptedFrame())
				{
					IPlugin* plugin = pluginsys->FindPluginByContext(iter.Context()->GetContext());
					if (plugin)
					{
						blame = plugin->GetFilename();
					}
					else
					{
						blame = iter.Context()->GetRuntime()->GetFilename();
					}
					break;
				}
			}
		}
		iter.Reset();
	}
	catch (std::exception e)
	{
		blame = "unknown";
	}
	sentry_value_t event = sentry_value_new_message_event(SENTRY_LEVEL_ERROR, blame, report.Message());
	sentry_setstrvalue(event, "server_name", "server_name")

	sentry_value_t error = sentry_value_new_object();
	sentry_setstrvalue(error, "type", "Exception")
	sentry_setstrvalue(error, "value", report.Message())
	sentry_setstrvalue(error, "module", blame)

	auto stacktraceList = DebugListener::GetStackTrace(iter);
	sentry_value_t frames = sentry_value_new_list();
	for (SPSentryFrame& trace : stacktraceList)
	{
		sentry_value_t traceObject = sentry_value_new_object();
		sentry_setstrvalue(traceObject, "function", trace.function)
		sentry_setstrvalue(traceObject, "filename", trace.filename)
		sentry_setstrvalue(traceObject, "module", blame)
		sentry_setintvalue(traceObject, "lineno", trace.lineno)
		sentry_value_append(frames, traceObject);
	}
	sentry_value_t stacktrace = sentry_value_new_object();
	sentry_value_set_by_key(stacktrace, "frames", frames);
	sentry_value_set_by_key(error, "stacktrace", stacktrace);
	sentry_value_set_by_key(event, "exception", error);
	
	sentry_capture_event(event);
}

/// <summary>
/// Get Frame information for sentry from the error. Modified version of SourceMod DebugReporter::GetStackTrace
/// </summary>
vector<SPSentryFrame> DebugListener::GetStackTrace(IFrameIterator &iter)
{
	vector<SPSentryFrame> trace;
	iter.Reset();

	if (!iter.Done())
	{
		for (int index = 0; !iter.Done(); iter.Next(), index++)
		{
			SPSentryFrame frame;
			const char* fn = iter.FunctionName();
			if (!fn)
			{
				fn = "<unknown function>";
			}
			if (iter.IsNativeFrame())
			{
				frame.lineno = 0;
				frame.filename = "<native>";
				frame.function = fn;
				trace.push_back(frame);
				continue;
			}
			if (iter.IsScriptedFrame())
			{
				const char* file = iter.FilePath();
				if (!file)
				{
					file = "<unknown>";
				}
				frame.lineno = iter.LineNumber();
				frame.filename = file;
				frame.function = fn;

				trace.push_back(frame);
			}
		}
	}

	return trace;
}

void DebugListener::OnContextExecuteError(IPluginContext* ctx, IContextTrace* error)
{
	//Not used
}
