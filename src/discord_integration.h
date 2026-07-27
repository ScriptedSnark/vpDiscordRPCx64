#ifdef DISCORD_INTEGRATION_H_RECURSE_GUARD
#error Recursive header files inclusion detected in discord_integration.h
#else //DISCORD_INTEGRATION_H_RECURSE_GUARD

#define DISCORD_INTEGRATION_H_RECURSE_GUARD

#ifndef DISCORD_INTEGRATION_H_GUARD
#define DISCORD_INTEGRATION_H_GUARD
#pragma once

enum DiscordRPC_Mode
{
	IDLE,
	EDITING,
	SETTINGS,
};

extern DiscordRPC_Mode mode;

void DiscordRPC_Init();
void DiscordRPC_Shutdown();

#endif //DISCORD_INTEGRATION_H_GUARD

#undef DISCORD_INTEGRATION_H_RECURSE_GUARD
#endif //DISCORD_INTEGRATION_H_RECURSE_GUARD
