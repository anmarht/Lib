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

#include "lib.h"
#include "helpers.h"
#if defined(_usewindows_) || (defined(_useposix_) && !defined(_useandroid_))
#include <stdio.h>
#include <string.h>
#endif
#include "general.h"
#include <stdarg.h>
#if defined(_useandroid_)
	#include <android/log.h>
	#include <stdio.h>
	#include <string.h>
#endif

mainns
	char _printtag[65]="\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

	void setprinttag(char *ptag)
	{
		strcpy(_printtag,ptag);
	}
	void print(const char * _Format, ...)
	{
		#if defined(_usewindows_) || (defined(_useposix_) && !defined(_useandroid_))
			va_list args;
			va_start(args,_Format);
			vprintf(_Format,args);
			va_end(args);
		#endif
		#if defined(_useandroid_)
			va_list args;
			va_start(args,_Format);
			__android_log_vprint(ANDROID_LOG_INFO, _printtag, _Format, args);
			va_end(args);

		#endif
	}
	void donothing(const char * _Format, ...)
	{

	}
nsend
