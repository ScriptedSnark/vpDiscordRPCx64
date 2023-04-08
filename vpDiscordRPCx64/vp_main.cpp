/*
 by ScriptedSnark.
 vp_main.cpp - Plugin initialization
*/

#include "includes.h"

//-----------------------------------------------------------------------------
// Initialize Discord RPC
//-----------------------------------------------------------------------------
void InitPlugin()
{
	// Do own things...

	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)DiscordRPC_MainThread, NULL, NULL, NULL); // Discord RPC
}

//-----------------------------------------------------------------------------
// Initialize our J.A.C.K plugin (REVERSE ENGINEERING INTENSIFIES)
//-----------------------------------------------------------------------------
#ifdef _M_X64
extern "C" int EXPORT vpMain(unsigned long long* Src, int version) // 64-bit implementation
{
	// TODO: memcpy(&unk_18001B4B0, Src, *Src); | other things for this

	setlocale(LC_ALL, "C");

	InitPlugin();

	return 0; // from IDA pseudocode
}
#else
extern "C" int EXPORT vpMain(DWORD* Src, int version) // 32-bit implementation
{
	if (*Src < 720u) // 0x2D0u -> 720u
		return -1;

	//memcpy(&unk_100170D8, Src, *Src);

	setlocale(LC_ALL, "C");

	InitPlugin();

	return 0;
}
#endif

DWORD WINAPI DllMain(_In_ void* _DllHandle, _In_ unsigned long _Reason, _In_opt_ void* _Reserved)
{
	if (_Reason == DLL_PROCESS_ATTACH)
		return 1;

	return 0;
}
