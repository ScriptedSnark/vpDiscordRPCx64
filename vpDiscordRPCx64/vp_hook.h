#ifdef HOOK_H_RECURSE_GUARD
#error Recursive header files inclusion detected in hook.h
#else //HOOK_H_RECURSE_GUARD

#define HOOK_H_RECURSE_GUARD

#ifndef HOOK_H_GUARD
#define HOOK_H_GUARD
#pragma once

typedef void (* _JACK_ConsolePrint)(char* args, ...);
extern _JACK_ConsolePrint ORIG_JACK_ConsolePrint;

void HookJACK();

#endif //HOOK_H_GUARD

#undef HOOK_H_RECURSE_GUARD
#endif //HOOK_H_RECURSE_GUARD
