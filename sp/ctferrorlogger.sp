#pragma semicolon 1

#include <ctferrorlogger>

public Plugin:myinfo = {
	name = "C.TF Error Logger Helper",
	author = "rob5300",
	version = "1.0"
}

public void OnPluginStart()
{
	RegConsoleCmd("sm_ctflogerror", cCTFErrorLog, "Log an error with the custom message. This is sent to the extension directly.");
	RegConsoleCmd("sm_ctferrortest", cErrorTest, "Create a sm error deliberatly. Is caught automatically via the extension.");
}

public Action cCTFErrorLog(int client, int args)
{
    char message[256];
    GetCmdArg(1, message, 256);
	
	//Native to create a new error on sentry with this custom message. Will include tags automatically + Log the calling plugin.
    CTFLogError(message);
}

public Action cErrorTest(int client, int args)
{
	//Make a sourcepawn error the normal way, the extension will catch this and log to sentry.
    ThrowError("Test Error");
}