/*
 by ScriptedSnark.
 log.cpp - getting plugin messages into editor console without doing it from the wrong thread

 I wouldn't do this if Qt4GUI could handle prints from separate thread but it doesn't...
*/

#include "includes.h"

struct logmsg_t
{
	bool warning;
	std::string text;
};

std::thread::id log_main_thread;
std::mutex log_mutex;
std::vector<logmsg_t> log_pending;
std::atomic<bool> log_has_pending(false);

#ifdef _WIN32
UINT_PTR log_timer = 0;

void CALLBACK Log_TimerProc(HWND, UINT, UINT_PTR, DWORD)
{
	Log_Flush();
}
#endif

void Log_Init()
{
	log_main_thread = std::this_thread::get_id();

#ifdef _WIN32
	log_timer = SetTimer(NULL, 0, LOG_FLUSH_INTERVAL, Log_TimerProc);
#endif
}

void Log_Shutdown()
{
#ifdef _WIN32
	if (log_timer)
	{
		KillTimer(NULL, log_timer);
		log_timer = 0;
	}
#endif
}

bool Log_OnMainThread()
{
	return std::this_thread::get_id() == log_main_thread;
}

void Log_Emit(bool warning, const char* text)
{
	printf("%s\n", text);

	if (!Log_OnMainThread())
	{
		logmsg_t message = { warning, text };

		std::lock_guard<std::mutex> lock(log_mutex);
		log_pending.push_back(message);
		log_has_pending = true;

		return;
	}

	Log_Flush();

	if (warning)
		Sys_Warning("%s", text);
	else
		Sys_Printf("%s", text);
}

void Log_Printf(const char* format, ...)
{
	char buffer[1024];
	va_list args;

	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	Log_Emit(false, buffer);
}

void Log_Warning(const char* format, ...)
{
	char buffer[1024];
	va_list args;

	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);

	Log_Emit(true, buffer);
}

void Log_Flush()
{
	std::vector<logmsg_t> pending;

	if (!Log_OnMainThread())
		return;

	if (!log_has_pending)
		return;

	std::lock_guard<std::mutex> lock(log_mutex);
	pending.swap(log_pending);
	log_has_pending = false;

	for (size_t i = 0; i < pending.size(); i++)
	{
		if (pending[i].warning)
			Sys_Warning("%s", pending[i].text.c_str());
		else
			Sys_Printf("%s", pending[i].text.c_str());
	}
}
