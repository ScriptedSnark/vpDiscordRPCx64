/*
 by ScriptedSnark.
 hook.cpp - hooking some functions in jack.exe
*/

#include "includes.h"
#include "MinHook.h"

_JACK_ConsolePrint ORIG_JACK_ConsolePrint = NULL;

//-----------------------------------------------------------------------------
// Hook J.A.C.K and its functions
//-----------------------------------------------------------------------------
void HookJACK()
{
	MH_Initialize();

	void* jackExe = GetModuleHandleA("jack.exe");
	if (!jackExe)
	{
		MessageBox(NULL, "Can't get module info about \"jack.exe\"! Exiting...", "vpDiscordRPCx64", MB_OK | MB_ICONERROR);
		exit(1);
		return;
	}

	//FindAsync(jackExe, patterns::JACK_ConsolePrint, ORIG_JACK_ConsolePrint);

	//MH_EnableHook(MH_ALL_HOOKS);
}