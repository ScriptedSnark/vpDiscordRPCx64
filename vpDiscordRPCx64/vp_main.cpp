/*
 by ScriptedSnark.
 vp_main.cpp - Plugin initialization
*/

#include "includes.h"

//-----------------------------------------------------------------------------
// Initialize our J.A.C.K plugin (REVERSE ENGINEERING INTENSIFIES)
// 
// Clean implementation of vpMain (x64):
// 
// extern "C" long long EXPORT vpMain(unsigned long long* argv, int version)
// {
//		if (version != 10)
//			return 10i64;
// 
//		//memcpy(&unk_18001B4B0, argv, *argv);
// 
//		setlocale(LC_ALL, "C");
// 
//		return 0i64;
// }
//-----------------------------------------------------------------------------
extern "C" long long EXPORT vpMain(unsigned long long* argv, int version)
{
#ifdef RETAIL
	if (version != 100)
#else
	if (version != 10)
#endif
		return 10i64; // from IDA pseudocode

	/* TODO: do memcpy, it may contain some useful funcs */
	//memcpy(&unk_18001B4B0, argv, *argv);

	setlocale(LC_ALL, "C");

	printf("========================\n");
	printf("https://github.com/ScriptedSnark/vpDiscordRPCx64\n");
	printf("Discord RPC plugin for J.A.C.K by ScriptedSnark\n");
	printf("Build timestamp: " __TIMESTAMP__ "\n");
	printf("========================\n");

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)DiscordRPC_MainThread, NULL, NULL, NULL); // Discord RPC

	return 0i64; // from IDA pseudocode
}

DWORD WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
	if (_Reason == DLL_PROCESS_ATTACH)
		return 1;

	return 0;
}