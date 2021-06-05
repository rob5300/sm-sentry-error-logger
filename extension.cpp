/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod CTFErrorLogger Extension
 * Creators.TF Robert Straub 2021
 * =============================================================================
 */
#include "extension.h"
#include <memory>
#include <string>
#include <DebugListener.h>
#include <IPluginSys.h>
#ifdef WIN
#define SENTRY_BUILD_STATIC
#endif
#include "sentry.h"
#include "CTFErrorLoggerConfig.h"

using namespace SourceMod;
using namespace std;

/**
 * @file extension.cpp
 * @brief Main extension object for the Error Logger
 */

//Sourcemod objects/pointers.
CTFErrorLogger g_Sample;
SMEXT_LINK(&g_Sample);
ITextParsers* textParsers;

DebugListener debugListener;
shared_ptr<CTFErrorLoggerConfig> config;

void CTFErrorLogger::Print(const char* toPrint)
{
	string newString = "[" + string(SMEXT_CONF_NAME) + "] " + string(toPrint) + "\n";
	printf(newString.c_str());
}

bool CTFErrorLogger::SDK_OnLoad(char* error, size_t maxlength, bool late)
{
    try
    {
        //Get the souremod text parser. Ignore vs errors its correct.
        SM_GET_IFACE (TEXTPARSERS, textParsers);

        //Load the config and continue if this was successful.
        config = make_shared<CTFErrorLoggerConfig> ();
        string path = string(smutils->GetSourceModPath());
#ifdef WIN
        path += "\\configs\\ctferrorlogger.cfg";
#else
        path += "/configs/ctferrorlogger.cfg";
#endif
        auto errorCode = textParsers->ParseFile_SMC(path.c_str(), config.get(), NULL);
        if (!errorCode == SMCError::SMCError_Okay)
        {
            string errorMessage = string ("Failed to parse config file, error was: ") + to_string(static_cast<int>(errorCode));
            Print (errorMessage.c_str());
            Print ((string ("Attempted Path was: ") + path).c_str());
            return false;
        }
        else
        {
            string successMessage = "Config Loaded! Server Name: [" + *config->server_name + "]";
            Print (successMessage.c_str());
        }

        //Setup sentry
        sentry_options_t *options = sentry_options_new ();
        sentry_options_set_dsn (options, config->sentry_dsn_url->c_str());
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
        
        return true;
    }
    catch (const exception &e)
    {
        Print (e.what());
        return false;
    }
}

void CTFErrorLogger::SDK_OnUnload()
{
	try
	{
		auto engine = g_pSM->GetScriptingEngine();
		engine->SetDebugListener(debugListener.oldListener);
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