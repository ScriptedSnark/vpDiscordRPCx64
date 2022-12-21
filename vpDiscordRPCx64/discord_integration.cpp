/*
 by rogender & ScriptedSnark.
 discord_integration.cpp - implementation of Discord Rich Presence for J.A.C.K
*/

#include "includes.h"

DiscordRPC_Mode mode = IDLE;

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

void DiscordRPC_MainThread()
{
	bool sendPresence = true;

	DiscordRichPresence presence;
	DiscordEventHandlers handlers;

	handlers.ready = handle_ready;
	handlers.errored = handle_errored;
	handlers.disconnected = handle_disconnected;

	Discord_Initialize(APPLICATION_ID, &handlers, 1, NULL);

	while (true)
	{
		HWND handle = FindWindowA(NULL, "J.A.C.K.");
		char buffer[128];
		GetWindowTextA(handle, buffer, sizeof(buffer));
		printf("Window name: %s\n", buffer);

		switch (mode)
		{
		default:
			break;

		case IDLE:
			if (sendPresence)
			{
				sendPresence = false;

				presence.details = "Idling";
				presence.largeImageKey = "jackhammer";
				presence.startTimestamp = time(0);

				Discord_UpdatePresence(&presence);
			}
			break;

			// TODO: implement EDITING case
		}

		Discord_RunCallbacks();
	}
}