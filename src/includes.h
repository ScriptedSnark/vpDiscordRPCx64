/**
 * by ScriptedSnark, 2022.
 * includes.h
 */

#ifdef INCLUDES_H_RECURSE_GUARD
#error Recursive header files inclusion detected in includes.h
#else //INCLUDES_H_RECURSE_GUARD

#define INCLUDES_H_RECURSE_GUARD

#ifndef INCLUDES_H_GUARD
#define INCLUDES_H_GUARD
#pragma once

#ifdef _WIN32
// WinAPI
#include <Windows.h>
#include <psapi.h>
#include <TlHelp32.h>
#endif

//STL
#include <string>
#include <vector>
#include <algorithm>
#include <thread>
#include <mutex>
#include <chrono>
#include <atomic>

#include <locale.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// J.A.C.K plugin SDK
#include "PluginAPI.h"
#include "PluginActions.h"

#include "discord_rpc.h"

#include "ini.h"
#include "log.h"
#include "settings.h"
#include "discord_integration.h"

#ifndef _WIN32
#include "x11.h"
#endif

#endif //INCLUDES_H_GUARD

#undef INCLUDES_H_RECURSE_GUARD
#endif //INCLUDES_H_RECURSE_GUARD
