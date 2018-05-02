/**************************************************************************
 *  Copyright 2008 Anmar Al-Tamimi <anmarhasan@gmail.com>
 *  
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *  
 *      http://www.apache.org/licenses/LICENSE-2.0
 *  
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 * 
 *  AHCLIB 2008
 */

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