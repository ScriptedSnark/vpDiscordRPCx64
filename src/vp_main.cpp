/*
 by ScriptedSnark.
 vp_main.cpp - Plugin initialization
*/

#include "includes.h"

plugin_funcs_t gEditorfuncs;

//-----------------------------------------------------------------------------
// Initialize Discord RPC
//-----------------------------------------------------------------------------
void InitPlugin()
{
	Settings_Load();
	DiscordRPC_Init();

	// let it be additional safety
	atexit(DiscordRPC_Shutdown);

	Log_Printf("vpDiscordRPC: loaded (%s)", gSettings.applicationId.c_str());
}

//-----------------------------------------------------------------------------
// Reapply vpDiscordRPC.ini without restarting editor
//-----------------------------------------------------------------------------
static void ReloadSettings(int state)
{
	Settings_Apply();

	Log_Printf("vpDiscordRPC: settings reloaded");
}

static void OpenSettings(int state)
{
	Log_Flush();

	Settings_OpenDialog();
}

pluginActionDesc_t settingsAction =
{
#if JACK_API_VERSION >= API_VERSION_STEAM_BETA
	"vpDiscordRPC_Settings",
#endif // JACK_API_VERSION >= API_VERSION_STEAM_BETA
	"&Settings...",
	"Edit the Discord Rich Presence settings",
	"Discord RPC",
#if JACK_API_VERSION <= API_VERSION_STEAM_PUBLIC
	0,
#endif // JACK_API_VERSION <= API_VERSION_STEAM_PUBLIC
	0,
	1,
	OpenSettings
};

pluginActionDesc_t reloadAction =
{
#if JACK_API_VERSION >= API_VERSION_STEAM_BETA
	"vpDiscordRPC_Reload",
#endif // JACK_API_VERSION >= API_VERSION_STEAM_BETA
	"&Reload settings",
	"Reloads " SETTINGS_FILENAME " and reconnects to Discord",
	"Discord RPC",
#if JACK_API_VERSION <= API_VERSION_STEAM_PUBLIC
	0,
#endif // JACK_API_VERSION <= API_VERSION_STEAM_PUBLIC
	0,
	1,
	ReloadSettings
};

extern "C" DLL_EXPORT int vpEnumActions(pfnRegisterAction registerAction, void* pluginManager)
{
	registerAction(&settingsAction, pluginManager);
	registerAction(&reloadAction, pluginManager);

	Log_Flush();

	return 2;
}

//-----------------------------------------------------------------------------
// Initialize our J.A.C.K plugin
//-----------------------------------------------------------------------------
extern "C" DLL_EXPORT int vpMain(plugin_funcs_t *editorFuncs, int editorPluginVersion)
{
	if (editorFuncs->m_intefaceVersion < sizeof(plugin_funcs_t))
		return -1;

	if (editorPluginVersion != PLUGIN_VERSION)
		return PLUGIN_VERSION;

	memcpy(&gEditorfuncs, editorFuncs, sizeof(gEditorfuncs));

	setlocale(LC_ALL, "C");

	Log_Init();
	InitPlugin();

	return 0;
}

#ifdef _WIN32

BOOL WINAPI DllMain(_In_ HINSTANCE _DllHandle, _In_ DWORD _Reason, _In_opt_ LPVOID _Reserved)
{
	if (_Reason == DLL_PROCESS_DETACH && _Reserved == NULL)
	{
		Log_Shutdown();
		DiscordRPC_Shutdown();
	}

	return TRUE;
}

#else

__attribute__((destructor)) static void vpShutdown()
{
	Log_Shutdown();
	DiscordRPC_Shutdown();
	X11_Shutdown();
}

#endif
