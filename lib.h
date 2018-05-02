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

// Namespace macro definitions
#define libroot alib
#define mainns \
	namespace libroot {
#define nsend }
#define ns1(x1) \
	mainns \
		namespace x1 { 
#define ns1end }}

#define ns2(x1,x2) \
	mainns \
		namespace x1 { \
			namespace x2 { 
#define ns2end }}}

// Debugging: uncomment the line below to enable debug prints
// #define _debug

// Please define below what system to use, the available are:
// #define _usewindows_
// #define _useposix_
// #define _useandroid_

#define _useposix_
#define _useios_

// Disabling CRT warnings.
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif


#if defined(_useposix_) 
	#include "lib.posix.h"
#endif

#if defined(_usewindows_) 
	#include "lib.windows.h"
#endif

                

