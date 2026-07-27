#ifdef SETTINGS_H_RECURSE_GUARD
#error Recursive header files inclusion detected in settings.h
#else //SETTINGS_H_RECURSE_GUARD

#define SETTINGS_H_RECURSE_GUARD

#ifndef SETTINGS_H_GUARD
#define SETTINGS_H_GUARD
#pragma once

#include <string>

#define SETTINGS_FILENAME "vpDiscordRPC.ini"

struct settings_t
{
	std::string applicationId;
	std::string largeImageKey;
	std::string largeImageText;
	std::string stateIdle;
	std::string stateEditing;
	std::string stateSettings;
	int updateInterval;
	bool showTimestamp;
};

extern settings_t gSettings;

void Settings_Load();
bool Settings_Save();
void Settings_Apply();
void Settings_OpenDialog();
void Settings_ReplaceToken(std::string& text, const char* token, const char* value);

#endif //SETTINGS_H_GUARD

#undef SETTINGS_H_RECURSE_GUARD
#endif //SETTINGS_H_RECURSE_GUARD
