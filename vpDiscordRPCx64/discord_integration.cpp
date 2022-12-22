/*
 by rogender & ScriptedSnark.
 discord_integration.cpp - implementation of Discord Rich Presence for J.A.C.K
*/

#include "includes.h"

DiscordRPC_Mode mode = IDLE;
HWND jack = NULL;
std::string mapname, editing_map;
bool dirty = false;

bool IsEditing() // bad solution, you need to hook something from J.A.C.K to get map name! 
{
	char buffer[64];
	GetWindowTextA(jack, buffer, sizeof(buffer));

	std::string windowname = buffer;
	std::string defaultname = "J.A.C.K.";

	if (defaultname == windowname)
		return false;

	mapname = windowname.substr(12);
	mapname.erase(remove(mapname.begin(), mapname.end(), ']'), mapname.end()); // :skull:

	editing_map = "Editing " + mapname;
	
	return true;
}

void handle_ready(const DiscordUser*)
{
	printf("Connected to Discord.\n");
}

void handle_errored(int error_code, const char* message)
{
	printf("Discord error (%d): %s\n", error_code, message);
}

void handle_disconnected(int error_code, const char* message)
{
	printf("Disconnected from Discord (%d): %s\n", error_code, message);
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
	DiscordRichPresence presence;
	DiscordEventHandlers handlers;

	handlers.ready = handle_ready;
	handlers.errored = handle_errored;
	handlers.disconnected = handle_disconnected;

	Discord_Initialize(APPLICATION_ID, &handlers, 1, NULL);

	presence.largeImageKey = "jackhammer";
	presence.startTimestamp = time(0);

	jack = FindWindow("QWidget", "J.A.C.K.");

	while (true)
	{
		// Without Sleep CPU usage will be around 25%
		Sleep(200); // TODO: replace it with async, I think?

		dirty = false;

		if (IsEditing())
			mode = EDITING;
		else
			mode = IDLE;

		switch (mode)
		{
		default:
			break;

		case IDLE:
			set_state(&presence, "Idling");
			set_details(&presence, NULL);
			break;

		case EDITING:
			set_state(&presence, editing_map.c_str());
			break;
		}

		update_presence(&presence);
		Discord_RunCallbacks();
	}
}