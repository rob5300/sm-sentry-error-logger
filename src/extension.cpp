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

//Sourcemod objects/pointers.
CTFErrorLogger g_Sample;
SMEXT_LINK(&g_Sample);

DebugListener debugListener;
shared_ptr<CTFErrorLoggerConfig> config;
unique_ptr<SMErrorLogReader> errorLogWatcher;
bool setup = false;

//Callback so we can setup the extension when the sentry url convar has been set.
//Required as the extension is loaded before convars exist.
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

//ConVars
ConVar ce_server_index("ce_server_index", "0", 0, "Server Numerical ID");
ConVar ce_sentry_dsn_url("ce_sentry_dsn_url", "", 0, "Sentry DSN URL", OnChangeCoreConVar);
ConVar ce_environment("ce_environment", "staging", 0, "Server Environment (staging/prod)");
ConVar ce_region("ce_region", "EU", 0, "Server Region");
ConVar ce_logreaderwaittime("ce_logreaderwaittime", "120", 0);
ConVar ce_type("ce_type", "", 0, "Server Type");

/** 
 * Class to allow our convars to be properly registered.
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

//Extension Class Definitions

void CTFErrorLogger::Print(const char* toPrint)
{
	string newString = "[" + string(SMEXT_CONF_NAME) + "] " + string(toPrint) + "\n";
	printf(newString.c_str());
}

bool CTFErrorLogger::TrySetup()
{
    if(setup || g_pCVar == nullptr) return false;

    auto cvar = g_pCVar->FindVar("ce_sentry_dsn_url");

    if(cvar != nullptr)
    {
        //Setup now if we have a value for the sentry url convar.
        string newDsnUrl = string(cvar->GetString());
        if (newDsnUrl.length() > 0 && !setup)
        {
            Setup();
            return true;
        }
    }
    else Print("Cvar 'ce_sentry_dsn_url' was null!");
    return false;
}

void CTFErrorLogger::Setup()
{
    //Make new config and give it the pointer to the ICvar.
    config = make_shared<CTFErrorLoggerConfig> (g_pCVar);
    //Setup sentry
    sentry_options_t *options = sentry_options_new ();
    sentry_options_set_dsn (options, config->sentry_dsn_url.c_str());
    auto loggerName = string(SMEXT_CONF_NAME) + string("_") + string(SMEXT_CONF_VERSION);
    sentry_options_set_release (options, loggerName.c_str());
    if (sentry_init(options) == 0)
    {
        Print ("Sentry Initalised!");
    }
    else
    {
        Print ((string("Sentry Did NOT Initialize, url used: ") + config->sentry_dsn_url).c_str());
    }

    //Add our debug listener
    auto spEngine = g_pSM->GetScriptingEngine ();
    if (spEngine != nullptr)
    {
        debugListener.config = config;
        debugListener.onError = [this] () {
            Print ("Error was logged");
        };
        auto oldListener = spEngine->SetDebugListener (&debugListener);

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
        errorLogWatcher = make_unique<SMErrorLogReader> (errorLogPath, config->logReaderWaitTime);
        errorLogWatcher->EventReciever = &debugListener;
        Print("ErrorLogReader was setup.");
    }

    setup = true;
}

bool CTFErrorLogger::SDK_OnLoad(char* error, size_t maxlength, bool late)
{
    if(!TrySetup())
    {
        Print("Delaying Setup until a value is found for 'ce_sentry_dsn_url'");
    }
    return true;
}

void CTFErrorLogger::SDK_OnUnload()
{
	if(errorLogWatcher != nullptr) errorLogWatcher->Stop();
	auto spEngine = g_pSM->GetScriptingEngine();
	if(debugListener.oldListener != nullptr) spEngine->SetDebugListener(debugListener.oldListener);
	sentry_close();
    ConVar_Unregister();
	Print("Unloaded extension and restored old Debug Listener");
    setup = false;
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
    const auto baseMessage = debugListener.GetBaseMessage (pluginFileName.c_str(), str);
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

    TrySetup();

    return true;
}
