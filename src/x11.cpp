// Maybe do something better with proper Wayland or other stuff support?
// ..............
#include "includes.h"

#ifndef _WIN32

#include <dlfcn.h>
#include <unistd.h>

// Xlib subset, declared here so building the plugin doesn't need the X11 headers
typedef struct _XDisplay Display;
typedef unsigned long XID;
typedef XID Window;
typedef unsigned long Atom;

typedef int (*XErrorHandler)(Display*, void*);

#define XA_CARDINAL	6
#define XA_STRING	31
#define XA_WINDOW	33

#define X_SUCCESS	0

static void* x11_lib = NULL;
static Display* x11_display = NULL;
static bool x11_failed = false;

static Display*		(*pfnXOpenDisplay)(const char*);
static int			(*pfnXCloseDisplay)(Display*);
static Window		(*pfnXDefaultRootWindow)(Display*);
static Atom			(*pfnXInternAtom)(Display*, const char*, int);
static int			(*pfnXGetWindowProperty)(Display*, Window, Atom, long, long, int, Atom, Atom*, int*, unsigned long*, unsigned long*, unsigned char**);
static int			(*pfnXFree)(void*);
static XErrorHandler(*pfnXSetErrorHandler)(XErrorHandler);

static XErrorHandler x11_prev_handler = NULL;

static Atom atom_client_list, atom_wm_pid, atom_net_wm_name, atom_wm_name, atom_utf8;

static int X11_ErrorHandler(Display* display, void* event)
{
	if (display == x11_display)
		return 0;

	if (x11_prev_handler)
		return x11_prev_handler(display, event);

	return 0;
}

static bool X11_Init()
{
	if (x11_display)
		return true;

	if (x11_failed)
		return false;

	x11_failed = true; // cleared once everything below worked out

	x11_lib = dlopen("libX11.so.6", RTLD_LAZY); // already loaded by Qt

	if (!x11_lib)
		return false;

	*(void**)&pfnXOpenDisplay = dlsym(x11_lib, "XOpenDisplay");
	*(void**)&pfnXCloseDisplay = dlsym(x11_lib, "XCloseDisplay");
	*(void**)&pfnXDefaultRootWindow = dlsym(x11_lib, "XDefaultRootWindow");
	*(void**)&pfnXInternAtom = dlsym(x11_lib, "XInternAtom");
	*(void**)&pfnXGetWindowProperty = dlsym(x11_lib, "XGetWindowProperty");
	*(void**)&pfnXFree = dlsym(x11_lib, "XFree");
	*(void**)&pfnXSetErrorHandler = dlsym(x11_lib, "XSetErrorHandler");

	if (!pfnXOpenDisplay || !pfnXCloseDisplay || !pfnXDefaultRootWindow || !pfnXInternAtom
		|| !pfnXGetWindowProperty || !pfnXFree || !pfnXSetErrorHandler)
		return false;

	// our own connection, so nothing here ever touches the one Qt polls on the main thread
	x11_display = pfnXOpenDisplay(NULL);

	if (!x11_display)
		return false;

	x11_prev_handler = pfnXSetErrorHandler(X11_ErrorHandler);

	atom_client_list = pfnXInternAtom(x11_display, "_NET_CLIENT_LIST", 0);
	atom_wm_pid = pfnXInternAtom(x11_display, "_NET_WM_PID", 0);
	atom_net_wm_name = pfnXInternAtom(x11_display, "_NET_WM_NAME", 0);
	atom_wm_name = pfnXInternAtom(x11_display, "WM_NAME", 0);
	atom_utf8 = pfnXInternAtom(x11_display, "UTF8_STRING", 0);

	x11_failed = false;

	return true;
}

static unsigned char* X11_GetProperty(Window window, Atom property, Atom type, unsigned long* count)
{
	Atom actualType;
	int actualFormat;
	unsigned long bytesAfter;
	unsigned char* data = NULL;

	*count = 0;

	if (pfnXGetWindowProperty(x11_display, window, property, 0, 0x7FFFFFFF, 0, type,
			&actualType, &actualFormat, count, &bytesAfter, &data) != X_SUCCESS)
		return NULL;

	if (!*count) // the property is simply not set on this window
	{
		if (data)
			pfnXFree(data);

		return NULL;
	}

	return data;
}

bool X11_GetWindowTitles(std::vector<std::string>& titles)
{
	unsigned long windowCount, count;
	unsigned char* clientList;
	Window* windows;

	if (!X11_Init())
		return false;

	// needs an EWMH window manager, every desktop worth the name has been one for well over a decade
	clientList = X11_GetProperty(pfnXDefaultRootWindow(x11_display), atom_client_list, XA_WINDOW, &windowCount);

	if (!clientList)
		return false;

	windows = (Window*)clientList;

	for (unsigned long i = 0; i < windowCount; i++)
	{
		unsigned char* pid = X11_GetProperty(windows[i], atom_wm_pid, XA_CARDINAL, &count);
		bool ours;

		if (!pid)
			continue;

		// 32 bit properties come back as longs, so this is right on both architectures
		ours = (*(unsigned long*)pid == (unsigned long)getpid());

		pfnXFree(pid);

		if (!ours)
			continue;

		unsigned char* name = X11_GetProperty(windows[i], atom_net_wm_name, atom_utf8, &count);

		if (!name)
			name = X11_GetProperty(windows[i], atom_wm_name, XA_STRING, &count); // pre-EWMH fallback

		if (!name)
			continue;

		titles.push_back(std::string((const char*)name, count));

		pfnXFree(name);
	}

	pfnXFree(clientList);

	return true;
}

void X11_Shutdown()
{
	if (!x11_display)
		return;

	pfnXSetErrorHandler(x11_prev_handler); // our handler is about to be unmapped
	x11_prev_handler = NULL;

	pfnXCloseDisplay(x11_display);
	x11_display = NULL;

	dlclose(x11_lib);
	x11_lib = NULL;
}

#endif //!_WIN32
