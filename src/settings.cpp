/*
 by ScriptedSnark.
 settings.cpp - vpDiscordRPC.ini parsing
*/

#ifndef _WIN32
#ifndef _GNU_SOURCE
#define _GNU_SOURCE // dladdr
#endif
#include <dlfcn.h>
#endif

#include "includes.h"

settings_t gSettings;

#ifdef _WIN32
extern "C" IMAGE_DOS_HEADER __ImageBase;
#endif

std::string GetPluginDirectory()
{
	std::string path;

#ifdef _WIN32
	char buffer[MAX_PATH];

	if (GetModuleFileNameA((HMODULE)&__ImageBase, buffer, sizeof(buffer)))
		path = buffer;
#else // LINUX
	Dl_info info;

	if (dladdr((void*)&GetPluginDirectory, &info) && info.dli_fname)
		path = info.dli_fname;
#endif

	size_t slash = path.find_last_of("/\\");

	if (slash == std::string::npos)
		return "";

	// ScriptedSnark: it should return directory where plugin is located
	return path.substr(0, slash + 1);
}

void Settings_ReplaceToken(std::string& text, const char* token, const char* value)
{
	size_t position = 0, length = strlen(token);

	while ((position = text.find(token, position)) != std::string::npos)
	{
		text.replace(position, length, value);
		position += strlen(value);
	}
}

void ReadString(inisec_t* section, const char* key, std::string& out)
{
	inikv_t* kv = INI_FindIKey(section, key);

	if (kv && kv->value)
		out = kv->value;
}

void ReadInt(inisec_t* section, const char* key, int& out)
{
	inikv_t* kv = INI_FindIKey(section, key);

	if (kv && kv->value)
		out = atoi(kv->value);
}

void ReadBool(inisec_t* section, const char* key, bool& out)
{
	inikv_t* kv = INI_FindIKey(section, key);

	if (kv && kv->value)
		out = (atoi(kv->value) != 0);
}

void Settings_SetDefaults()
{
	gSettings.applicationId = "967069834728849449";
	gSettings.largeImageKey = "jackhammer";
	gSettings.largeImageText = "Just Another Creation Kit (J.A.C.K)";
	gSettings.stateIdle = "Idling";
	gSettings.stateEditing = "Editing {map}";
	gSettings.stateSettings = "Configuring";
	gSettings.updateInterval = 200;
	gSettings.showTimestamp = true;
}

void Settings_Load()
{
	inifile_t file;
	std::string path = GetPluginDirectory() + SETTINGS_FILENAME;

	Settings_SetDefaults();

	if (INI_LoadFile(path.c_str(), &file, Log_Warning) != ifls_success)
	{
		Log_Printf("vpDiscordRPC: using default settings");
	}
	else
	{
		inisec_t* discord = INI_FindISection(&file, "Discord");
		inisec_t* presence = INI_FindISection(&file, "Presence");

		ReadString(discord, "applicationId", gSettings.applicationId);
		ReadString(discord, "largeImageKey", gSettings.largeImageKey);
		ReadString(discord, "largeImageText", gSettings.largeImageText);
		ReadInt(discord, "updateInterval", gSettings.updateInterval);
		ReadBool(discord, "showTimestamp", gSettings.showTimestamp);

		ReadString(presence, "idle", gSettings.stateIdle);
		ReadString(presence, "editing", gSettings.stateEditing);
		ReadString(presence, "settings", gSettings.stateSettings);

		INI_FreeFile(&file);
	}

	// atyatya)
	if (gSettings.updateInterval < 100)
		gSettings.updateInterval = 100;
}

bool Settings_Save()
{
	std::string path = GetPluginDirectory() + SETTINGS_FILENAME;
	FILE* file = fopen(path.c_str(), "wb");

	if (!file)
	{
		Log_Warning("vpDiscordRPC: can't write \"%s\"", path.c_str());
		return false;
	}

	fprintf(file, "; vpDiscordRPC settings\n\n");

	fprintf(file, "[Discord]\n");
	fprintf(file, "applicationId = %s\n", gSettings.applicationId.c_str());
	fprintf(file, "largeImageKey = %s\n", gSettings.largeImageKey.c_str());
	fprintf(file, "; {version} is replaced with editor version string\n");
	fprintf(file, "largeImageText = %s\n", gSettings.largeImageText.c_str());
	fprintf(file, "; how often the presence is refreshed, in milliseconds (min. 100)\n");
	fprintf(file, "updateInterval = %d\n", gSettings.updateInterval);
	fprintf(file, "; show \"elapsed\" timer\n");
	fprintf(file, "showTimestamp = %d\n\n", gSettings.showTimestamp ? 1 : 0);

	fprintf(file, "[Presence]\n");
	fprintf(file, "idle = %s\n", gSettings.stateIdle.c_str());
	fprintf(file, "; {map} is replaced with name of map being edited\n");
	fprintf(file, "editing = %s\n", gSettings.stateEditing.c_str());
	fprintf(file, "settings = %s\n", gSettings.stateSettings.c_str());

	fclose(file);

	return true;
}

void Settings_Apply()
{
	DiscordRPC_Shutdown();
	Settings_Load();
	DiscordRPC_Init();
}

//-----------------------------------------------------------------------------
// Settings dialog
//-----------------------------------------------------------------------------
void Settings_QueryString(const char* controlName, std::string& out)
{
	char buffer[256] = { 0 };

	Dialog_QueryArgument(controlName, buffer, sizeof(buffer));
	out = buffer;
	out.erase(remove(out.begin(), out.end(), ';'), out.end()); // a ';' would come back as a comment on the next load
}

void Settings_DialogSave()
{
	Settings_QueryString("applicationId", gSettings.applicationId);
	Settings_QueryString("largeImageKey", gSettings.largeImageKey);
	Settings_QueryString("largeImageText", gSettings.largeImageText);

	gSettings.updateInterval = Dialog_QueryArgumentInt("updateInterval");
	gSettings.showTimestamp = (Dialog_QueryArgumentInt("showTimestamp") != 0);

	Settings_QueryString("stateIdle", gSettings.stateIdle);
	Settings_QueryString("stateEditing", gSettings.stateEditing);
	Settings_QueryString("stateSettings", gSettings.stateSettings);

	if (!Settings_Save())
	{
		Dialog_Printf("Failed to save " SETTINGS_FILENAME ".\nIs it locked?");
		return;
	}
	Settings_Apply();

	Log_Flush();
	Dialog_Printf("Saved to " SETTINGS_FILENAME);
}

void Settings_OpenDialog()
{
	Dialog_Begin("Discord RPC Settings");
	{
		Dialog_AddTextEdit("applicationId", "Application ID", gSettings.applicationId.c_str(), 0);
		Dialog_AddTextEdit("largeImageKey", "Large image key", gSettings.largeImageKey.c_str(), 0);
		Dialog_AddTextEdit("largeImageText", "Large image text", gSettings.largeImageText.c_str(), 0);

		Dialog_AddSpinBox("updateInterval", "Update interval (ms)", gSettings.updateInterval, 100, 5000, 50, 0);
		Dialog_AddCheckBox("showTimestamp", "Show elapsed time", gSettings.showTimestamp, 0);

		Dialog_AddTextEdit("stateIdle", "Idle", gSettings.stateIdle.c_str(), 0);
		Dialog_AddTextEdit("stateEditing", "Editing a map", gSettings.stateEditing.c_str(), 0);
		Dialog_AddTextEdit("stateSettings", "Editor settings open", gSettings.stateSettings.c_str(), 0);

		Dialog_InitInternalCommand("&Save", Settings_DialogSave);
	}
	Dialog_End();
}
