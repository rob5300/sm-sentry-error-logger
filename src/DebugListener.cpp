#include "DebugListener.h"
#include "iconvar.h"

using namespace SourcePawn;
using namespace std;

#define sentry_setstrvalue(object, strkey, strvalue) sentry_value_set_by_key(object, strkey, sentry_value_new_string(strvalue));
#define sentry_setintvalue(object, strkey, intvalue) sentry_value_set_by_key(object, strkey, sentry_value_new_int32(intvalue));

void DebugListener::OnDebugSpew (const char *msg, ...)
{
    if (msg != nullptr)
    {
		if (oldListener)
		{
			oldListener->OnDebugSpew(msg);
		}
		if(onError) onError();
		const sentry_value_t event = GetBaseMessage ("Debug", msg);
		sentry_capture_event(event);
	}
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

	if(onError) onError();
    iter.Reset ();
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
            for (; !iter.Done (); iter.Next ())
            {
                if (iter.IsScriptedFrame ())
                {
                    const char* filePath = iter.FilePath ();
                    if(filePath != nullptr) printf (filePath); 
                    IPlugin *plugin = plsys->FindPluginByContext (iter.Context ()->GetContext ());
                    if (plugin != nullptr)
                    {
                        blame = plugin->GetPublicInfo()->name;
                    }
                    else
                    {
                        blame = iter.Context ()->GetRuntime ()->GetFilename ();
                    }
                    break;
                }
            }
		}
		iter.Reset();

		if (blame == nullptr)
		{
            blame = "unknown_module";
		}
	}
	catch (std::exception e)
	{
		blame = "unknown_module";
	}
	
	sentry_value_t event = GetBaseMessage (blame, report.Message ());

	sentry_value_t error = sentry_value_new_object();
	sentry_setstrvalue(error, "type", "Exception")
	sentry_setstrvalue(error, "value", report.Message())
	sentry_setstrvalue(error, "module", blame)

	//Construct the stack trace manually using the event data.
	auto stacktraceList = DebugListener::GetStackTrace(iter);
    if (stacktraceList.size() > 0)
    {
		sentry_value_t frames = sentry_value_new_list();
		for (SPSentryFrame& trace : stacktraceList)
		{
			sentry_value_t traceObject = sentry_value_new_object();
			sentry_setstrvalue(traceObject, "function", trace.function.c_str())
			//sentry_setstrvalue(traceObject, "filename", trace.filename.c_str())
			sentry_setstrvalue(traceObject, "abs_path", trace.filename.c_str())
			sentry_setstrvalue(traceObject, "module", blame)
			sentry_setstrvalue(traceObject, "package", blame)
			sentry_setintvalue(traceObject, "lineno", trace.lineno)
			sentry_value_append(frames, traceObject);
		}
		sentry_value_t stacktrace = sentry_value_new_object();
		sentry_value_set_by_key(stacktrace, "frames", frames);
		sentry_value_set_by_key(error, "stacktrace", stacktrace);
		sentry_value_set_by_key(event, "exception", error);
	}
	
	sentry_capture_event(event);
}


vector<SPSentryFrame> DebugListener::GetStackTrace(IFrameIterator &iter)
{
	vector<SPSentryFrame> trace;
	iter.Reset();

	if (!iter.Done())
	{
		for (int index = 0; !iter.Done(); iter.Next(), index++)
		{
			SPSentryFrame frame;
            string functionName = string ("unknownfunc");
			const char* fn = iter.FunctionName();
			if (fn)
			{
				functionName = string(fn);
			}
			if (iter.IsNativeFrame())
			{
				frame.lineno = 0;
				frame.filename = string("<native>");
                frame.function = string(fn);
				trace.push_back(frame);
				continue;
			}
			if (iter.IsScriptedFrame())
			{
                const char *fileNameSrc = iter.FilePath ();
                string file = string ("<unknown file>");
                if (fileNameSrc != nullptr)
				{
                    file = string (fileNameSrc);
				}
				frame.lineno = iter.LineNumber();
				frame.filename = file;
				string functionName = string (fn) + string("()");
				frame.function = functionName;

				trace.push_back(frame);
			}
		}
	}

	return trace;
}

sentry_value_t DebugListener::GetBaseMessage (const char *blame, const char *message)
{
	sentry_value_t event = sentry_value_new_message_event (SENTRY_LEVEL_ERROR, blame, message);
    sentry_setstrvalue (event, "server_name", config->server_name->GetString());
    sentry_setstrvalue (event, "environment", config->environment->GetString());

    sentry_value_t tags = sentry_value_new_object ();
    sentry_setstrvalue (tags, "server_id", config->server_id->GetString());
    sentry_setstrvalue (tags, "region", config->region->GetString());
	sentry_setstrvalue (tags, "server_type", config->type->GetString());

	//Add the servers current map as a tag.
	string currentMap = "unknown_map";
	if (gamehelpers != nullptr)
	{
		currentMap = gamehelpers->GetCurrentMap();
	}
	sentry_setstrvalue(tags, "map", currentMap.c_str());

    sentry_value_set_by_key (event, "tags", tags);
	return event;
}

void DebugListener::OnSMErrorFound (std::string &error)
{
	auto event = GetBaseMessage("SMErrorLog", error.c_str());
	sentry_capture_event(event);
}

void DebugListener::OnContextExecuteError(IPluginContext* ctx, IContextTrace* error)
{
	//Not used
}
