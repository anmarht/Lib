#pragma once
// Including SDKDDKVer.h defines the highest available Windows platform.

// If you wish to build your application for a previous Windows platform, include WinSDKVer.h and
// set the _WIN32_WINNT macro to the platform you wish to support before including SDKDDKVer.h.

#ifndef Libroot_WINDOWS_
#define Libroot_WINDOWS_

#include <SDKDDKVer.h>


#ifndef _UNICODE  // If not unicode it means this is VC6 so define all things missing. Anmar Hassan
#define _UNICODE
#define UNICODE

//#define LONG_PTR long
#define MAPVK_VK_TO_VSC     (0)
#endif

#define WIN32_LEAN_AND_MEAN 

#include <stdio.h>
#include <tchar.h>
#include <Windows.h>

#endif