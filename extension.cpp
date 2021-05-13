/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod CTFErrorLogger Extension
 * Copyright (C) 2004-2008 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */
#define SENTRY_BUILD_STATIC 1
#include "extension.h"
#include <string>
#include <DebugListener.h>
#include <sentry.h>
#include <IPluginSys.h>

using namespace SourceMod;
using namespace std;

/**
 * @file extension.cpp
 * @brief Implement extension code here.
 */

CTFErrorLogger g_Sample;		/**< Global singleton for extension's main interface */
SMEXT_LINK(&g_Sample);
IPluginManager* g_pPlugins = NULL;
DebugListener debugListener;

void CTFErrorLogger::Print(const char* toPrint)
{
	string newString = "[" + string(SMEXT_CONF_NAME) + "] " + string(toPrint) + "\n";
	printf(newString.c_str());
}

bool CTFErrorLogger::SDK_OnLoad(char* error, size_t maxlength, bool late)
{
	SM_GET_IFACE(PLUGINSYSTEM, g_pPlugins);

	sentry_options_t* options = sentry_options_new();
	sentry_options_set_dsn(options, "https://68a19ade4e68415e9827e30c44d384b0@sentry.creators.tf/4");
	sentry_options_set_release(options, SMEXT_CONF_NAME);
	sentry_init(options);
	Print("Sentry Initalised!");

	auto engine = g_pSM->GetScriptingEngine();
	
	if (engine != nullptr)
	{
		debugListener.onError = [this](const char* message) { Print("Error was logged"); };
		auto oldListener = engine->SetDebugListener(&debugListener);
		debugListener.oldListener = oldListener;

		Print("Added Debug Listener");
	}

	return true;
}

void CTFErrorLogger::SDK_OnUnload()
{
	try
	{
		auto engine = g_pSM->GetScriptingEngine();
		engine->SetDebugListener(debugListener.oldListener);
		sentry_close();
		Print("Unloaded extensiona and restored old Debug Listener");
	}
	catch (exception e)
	{
		Print(strcat("Things may break, Failed to fully unload due to: ", e.what()));
	}
}