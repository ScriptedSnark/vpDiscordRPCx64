#ifdef DISCORD_INTEGRATION_H_RECURSE_GUARD
#error Recursive header files inclusion detected in discord_integration.h
#else //DISCORD_INTEGRATION_H_RECURSE_GUARD

#define DISCORD_INTEGRATION_H_RECURSE_GUARD

#ifndef DISCORD_INTEGRATION_H_GUARD
#define DISCORD_INTEGRATION_H_GUARD
#pragma once

constexpr char APPLICATION_ID[] = "967069834728849449";

enum DiscordRPC_Mode
{
	IDLE,
	EDITING,
};

extern DiscordRPC_Mode mode;

void DiscordRPC_MainThread();

#endif //DISCORD_INTEGRATION_H_GUARD

#undef DISCORD_INTEGRATION_H_RECURSE_GUARD
#endif //DISCORD_INTEGRATION_H_RECURSE_GUARD