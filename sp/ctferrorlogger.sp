#pragma semicolon 1

#include <ctferrorlogger>

public Plugin:myinfo = {
	name = "C.TF Error Logger Helper",
	author = "rob5300",
	version = "1.0"
}

public void OnPluginStart()
{
	RegConsoleCmd("sm_ctferrorlog", cCTFErrorLog, "Log an error with the custom message");
}

public Action cCTFErrorLog(int client, int args)
{
    char message[256];
    GetCmdArg(1, message, 256);
    CTFLogError(message);
}