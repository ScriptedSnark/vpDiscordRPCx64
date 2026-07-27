#ifdef LOG_H_RECURSE_GUARD
#error Recursive header files inclusion detected in log.h
#else //LOG_H_RECURSE_GUARD

#define LOG_H_RECURSE_GUARD

#ifndef LOG_H_GUARD
#define LOG_H_GUARD
#pragma once

#define LOG_FLUSH_INTERVAL 250 // ms, Windows only, see Log_Init

void Log_Init();
void Log_Shutdown();
void Log_Printf(const char* format, ...);
void Log_Warning(const char* format, ...);
void Log_Flush();

#endif //LOG_H_GUARD

#undef LOG_H_RECURSE_GUARD
#endif //LOG_H_RECURSE_GUARD
