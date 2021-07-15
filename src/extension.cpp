/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod CTFErrorLogger Extension
 * Creators.TF Robert Straub 2021
 * =============================================================================
 */
#include <memory>
#include <string>
#include <filesystem>
#include <IPluginSys.h>
#ifdef _WIN32
#define SENTRY_BUILD_STATIC
#endif
#include "extension.h"
#include "tier1/iconvar.h"
#include "convar.h"
#include "icvar.h"
#include "CTFErrorLoggerConfig.h"
#include "SMErrorLogReader.h"
#include "lib/sentry.h"
#include "DebugListener.h"

using namespace SourceMod;
using namespace std;

/**
 * @file extension.cpp
 * @brief Main extension object for the Error Logger
 */

//Sourcemod objects/pointers.
CTFErrorLogger g_Sample;
SMEXT_LINK(&g_Sample);

DebugListener debugListener;
shared_ptr<CTFErrorLoggerConfig> config;
unique_ptr<SMErrorLogReader> errorLogWatcher;

//Forward declare the function
void OnChangeCoreConVar ( IConVar *var, const char *pOldValue, float flOldValue );

ConVar ce_server_index("ce_server_index", "0", 0, "Server Numerical ID");
ConVar ce_sentry_dsn_url("ce_sentry_dsn_url", "", 0, "Sentry DSN URL", OnChangeCoreConVar);
ConVar ce_server_name("ce_server_name", "", 0, "Server Name for Sentry");
ConVar ce_environment("ce_environment", "staging", 0, "Server Environment (staging/prod)");
ConVar ce_region("ce_region", "EU", 0, "Server Region");
ConVar ce_logreaderwaittime("ce_logreaderwaittime", "30", 0);

bool setup = false;

/** 
 * Something like this is needed to register cvars/CON_COMMANDs.
 */
class BaseAccessor : public IConCommandBaseAccessor
{
public:
	bool RegisterConCommandBase(ConCommandBase *pCommandBase)
	{
		/* Always call META_REGCVAR instead of going through the engine. */
		return META_REGCVAR(pCommandBase);
	}
} s_BaseAccessor;

//Callback so we can setup the extension when the sentry url convar has been set.
void OnChangeCoreConVar ( IConVar *var, const char *pOldValue, float flOldValue )
{
	string newDsnUrl = string(((ConVar*)var)->GetString());
    if (newDsnUrl.length() > 0 && !setup)
    {
        auto me = reinterpret_cast<CTFErrorLogger*>(myself);
        me->Print("ConVar value found for sentry_dsn_url, can setup.");
        me->Setup();
    }
}

void CTFErrorLogger::Print(const char* toPrint)
{
	string newString = "[" + string(SMEXT_CONF_NAME) + "] " + string(toPrint) + "\n";
	printf(newString.c_str());
}

void CTFErrorLogger::Setup()
{
    //Make new config and give it the pointer to the ICvar.
    config = make_shared<CTFErrorLoggerConfig> (g_pCVar);
    //Setup sentry
    sentry_options_t *options = sentry_options_new ();
    sentry_options_set_dsn (options, config->sentry_dsn_url.c_str());
    sentry_options_set_release (options, SMEXT_CONF_NAME);
    sentry_init (options);
    Print ("Sentry Initalised!");

    //Add our debug listener
    auto engine = g_pSM->GetScriptingEngine ();
    if (engine != nullptr)
    {
        debugListener.config = config;
        debugListener.onError = [this] () {
            Print ("Error was logged");
        };
        auto oldListener = engine->SetDebugListener (&debugListener);

        //Assign old listener so our new one can forward events back to it.
        debugListener.oldListener = oldListener;

        Print ("Added Debug Listener");
    }

    //Setup error log watcher
    string errorLogPath = string(smutils->GetSourceModPath());
    if (errorLogPath.length() > 0)
    {
#ifdef _WIN32
        errorLogPath += "\\logs";
#else
        errorLogPath += "/logs";
#endif
        if (config->logReaderWaitTime != 0)
        {
            errorLogWatcher = make_unique<SMErrorLogReader> (errorLogPath, config->logReaderWaitTime);
            errorLogWatcher->EventReciever = &debugListener;
            Print("ErrorLogReader was setup.");
        }
        else
        {
            Print("ErrorLogReader was NOT setup, as a wait time was missing. (0).");
        }
    }

    setup = true;
}

bool CTFErrorLogger::SDK_OnLoad(char* error, size_t maxlength, bool late)
{
    //Setup now if we have a value for the sentry url convar.
    string newDsnUrl = string(g_pCVar->FindVar("ce_sentry_dsn_url")->GetString());
    if (newDsnUrl.length() > 0 && !setup)
    {
        Setup();
    }
    else
    {
        Print("Delaying setup");
    }
    return true;
}

void CTFErrorLogger::SDK_OnUnload()
{
	try
	{
        if(errorLogWatcher != nullptr) errorLogWatcher->Stop();
		auto engine = g_pSM->GetScriptingEngine();
		if(debugListener.oldListener != nullptr) engine->SetDebugListener(debugListener.oldListener);
		sentry_close();
		Print("Unloaded extension and restored old Debug Listener");
	}
	catch (exception &e)
	{
		Print(strcat("Things may break, Failed to fully unload due to: ", e.what()));
	}
}

//Natives
cell_t sm_CTFLogError (IPluginContext *pContext, const cell_t *params)
{
    IPlugin* plugin = plsys->FindPluginByContext (pContext->GetContext());
    string pluginFileName;
    if (plugin != nullptr)
    {
        pluginFileName = plugin->GetPublicInfo()->name;
    }
    else
    {
        pluginFileName = "unknown_plugin";
    }
    char *str;
    pContext->LocalToString (params [1], &str);
    auto baseMessage = debugListener.GetBaseMessage (pluginFileName.c_str(), str);
    sentry_capture_event (baseMessage);
    string printMessage = string (SMEXT_CONF_NAME) + " captured a manual error: " + string (str);
    printf (printMessage.c_str());
    return 1;
}

const sp_nativeinfo_t NativeFunctions [] = {
    {"CTFLogError", &sm_CTFLogError},
    {NULL, NULL},
};

void CTFErrorLogger::SDK_OnAllLoaded ()
{
    sharesys->AddNatives (myself, NativeFunctions);
}

bool CTFErrorLogger::SDK_OnMetamodLoad(ISmmAPI* ismm, char* error, size_t maxlen, bool late)
{
    GET_V_IFACE_ANY(GetEngineFactory, g_pCVar, ICvar, CVAR_INTERFACE_VERSION);
    ConVar_Register(0, &s_BaseAccessor);

    //ConVarRef cvar("ce_server_index");
    //ConVar *ce_server_indexa = g_pCVar->FindVar("ce_server_index");
    //string cvarMsg = string("!!!ce_server_index is: ") + cvar.GetString();
    //Print(cvarMsg.c_str());

    return true;
}
