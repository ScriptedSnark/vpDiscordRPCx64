/*
 by ScriptedSnark.
 vp_main.cpp - Plugin initialization
*/

#include "includes.h"

//-----------------------------------------------------------------------------
// Initialize our J.A.C.K plugin (REVERSE ENGINEERING INTENSIFIES)
//-----------------------------------------------------------------------------
extern "C" long long EXPORT vpMain(unsigned long long* argv, int version)
{
	if (version != 10)
		return 10i64; // from IDA pseudocode

	/* TODO: do memcpy, it may contain some useful funcs */
	//memcpy(&unk_18001B4B0, argv, *argv);

	setlocale(LC_ALL, "C");

	AllocConsole();
	FILE* in, *out;
	freopen_s(&in, "conin$", "r", stdin);
	freopen_s(&out, "conout$", "w+", stdout);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, 11);

	printf("========================\n");
	printf("Discord RPC plugin for J.A.C.K by ScriptedSnark\n");
	printf("Build timestamp: " __TIMESTAMP__ "\n");
	printf("========================\n");

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)DiscordRPC_MainThread, NULL, NULL, NULL); // Discord RPC

	return 0i64; // from IDA pseudocode
}

DWORD WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
	if (_Reason == DLL_PROCESS_ATTACH)
	{
		HookJACK();
		return 1;
	}

	return 0;
}