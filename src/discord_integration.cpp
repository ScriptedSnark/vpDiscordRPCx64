/*
 by rogender & ScriptedSnark.
 discord_integration.cpp - implementation of Discord Rich Presence for J.A.C.K
*/

#include "includes.h"

#define EDITOR_TITLE_PREFIX "J.A.C.K. - ["
// ScriptedSnark: FIXME: localize
#define SETTINGS_TITLE "Configure J.A.C.K."

DiscordRPC_Mode mode = IDLE;
std::string mapname, editing_map;
bool dirty = false;

// ScriptedSnark: this is why I ship Discord RPC sources
// Actual repo doesn't have these
const DiscordButton rpc_buttons[] = {
	{ "Download J.A.C.K", "https://jack.hlfx.ru/en/download.html" },
	{ "Download RPC plugin", "https://github.com/ScriptedSnark/vpDiscordRPCx64" },
	{ NULL, NULL }
};

std::string large_image_text;

bool rpc_running = false;

// bad solution, you need to hook something from J.A.C.K to get map name!
bool ParseMapName(const std::string& windowname)
{
	size_t prefix = strlen(EDITOR_TITLE_PREFIX);

	if (windowname.compare(0, prefix, EDITOR_TITLE_PREFIX) != 0)
		return false;

	size_t bracket = windowname.find_last_of(']');

	if (bracket == std::string::npos || bracket < prefix)
		return false;

	mapname = windowname.substr(prefix, bracket - prefix);

	editing_map = gSettings.stateEditing;
	Settings_ReplaceToken(editing_map, "{map}", mapname.c_str());

	return true;
}

#ifdef _WIN32
HWND jack = NULL;

bool IsEditing()
{
	char buffer[128] = { 0 };

	if (jack == NULL)
		return false;

	GetWindowTextA(jack, buffer, sizeof(buffer));

	return ParseMapName(buffer);
}

bool IsInSettings()
{
	HWND settings = FindWindow(0, SETTINGS_TITLE);

	if (settings == NULL)
		return false;

	return true;		
}
#else
bool IsEditing()
{
	std::vector<std::string> titles;

	if (!X11_GetWindowTitles(titles))
		return false;

	for (size_t i = 0; i < titles.size(); i++)
	{
		if (ParseMapName(titles[i]))
			return true;
	}

	return false;
}

bool IsInSettings()
{
	std::vector<std::string> titles;

	if (!X11_GetWindowTitles(titles))
		return false;

	for (size_t i = 0; i < titles.size(); i++)
	{
		if (titles[i] == SETTINGS_TITLE)
			return true;
	}

	return false;
}
#endif

void handle_ready(const DiscordUser*)
{
	Log_Printf("Connected to Discord.");
}

void handle_errored(int error_code, const char* message)
{
	Log_Warning("Discord error (%d): %s", error_code, message);
}

void handle_disconnected(int error_code, const char* message)
{
	Log_Warning("Disconnected from Discord (%d): %s", error_code, message);
}

void set_state(DiscordRichPresence* presence, const char* state)
{
	presence->state = state;
	dirty = true;
}

void set_details(DiscordRichPresence* presence, const char* details)
{
	presence->details = details;
	dirty = true;
}

void update_presence(const DiscordRichPresence* presence)
{
	if (dirty)
		Discord_UpdatePresence(presence);
}

void DiscordRPC_MainThread()
{
	DiscordRichPresence presence = {};

	presence.largeImageKey = gSettings.largeImageKey.c_str();
	presence.largeImageText = large_image_text.c_str();
	presence.buttons = rpc_buttons;

	if (gSettings.showTimestamp)
		presence.startTimestamp = time(0);

#ifdef _WIN32
	jack = FindWindow("QWidget", "J.A.C.K.");
#endif

	while (rpc_running)
	{
#ifdef _WIN32
		Sleep(gSettings.updateInterval);
#else
		std::this_thread::sleep_for(std::chrono::milliseconds(gSettings.updateInterval));
#endif

		dirty = false;

		if (IsInSettings())  // Settings needs to be first cause can't edit map with settings open
			mode = SETTINGS;
		else if (IsEditing())
			mode = EDITING;
		else
			mode = IDLE;

		switch (mode)
		{
		default:
			break;

		case IDLE:
			set_state(&presence, gSettings.stateIdle.c_str());
			set_details(&presence, NULL);
			break;

		case EDITING:
			set_state(&presence, editing_map.c_str());
			break;

		case SETTINGS:
			set_state(&presence, gSettings.stateSettings.c_str());
			break;
		}

		update_presence(&presence);
		Discord_RunCallbacks();
	}
}

void DiscordRPC_Init()
{
	DiscordEventHandlers handlers = {};

	if (rpc_running)
		return;

	// ScriptedSnark: expanded here so the token itself is what stays in our .ini
	// and V_VersionString is touched from main thread
	large_image_text = gSettings.largeImageText;
	Settings_ReplaceToken(large_image_text, "{version}", V_VersionString());

	handlers.ready = handle_ready;
	handlers.errored = handle_errored;
	handlers.disconnected = handle_disconnected;

	Discord_Initialize(gSettings.applicationId.c_str(), &handlers, 1, NULL);

	rpc_running = true;

#ifdef _WIN32
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)DiscordRPC_MainThread, NULL, NULL, NULL);
#else
	std::thread(DiscordRPC_MainThread).detach();
#endif
}

void DiscordRPC_Shutdown()
{
	if (!rpc_running)
		return;

	rpc_running = false;

#ifdef _WIN32
	Sleep(gSettings.updateInterval);
#else
	std::this_thread::sleep_for(std::chrono::milliseconds(gSettings.updateInterval));
#endif

	Discord_ClearPresence();
	Discord_Shutdown();
}

