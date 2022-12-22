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

// WinAPI
#include <Windows.h>
#include <psapi.h>
#include <TlHelp32.h>

//STL
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <array>

#include <locale.h>
#include <time.h>

#include "discord-rpc.h"
#include "discord_integration.h"

#include "patterns.hpp"
#include "vp_hook.h"

#define EXPORT __declspec(dllexport)

#endif //INCLUDES_H_GUARD

#undef INCLUDES_H_RECURSE_GUARD
#endif //INCLUDES_H_RECURSE_GUARD