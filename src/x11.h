#ifdef X11_H_RECURSE_GUARD
#error Recursive header files inclusion detected in x11.h
#else //X11_H_RECURSE_GUARD

#define X11_H_RECURSE_GUARD

#ifndef X11_H_GUARD
#define X11_H_GUARD
#pragma once

#include <string>
#include <vector>

bool X11_GetWindowTitles(std::vector<std::string>& titles);
void X11_Shutdown();

#endif //X11_H_GUARD

#undef X11_H_RECURSE_GUARD
#endif //X11_H_RECURSE_GUARD
