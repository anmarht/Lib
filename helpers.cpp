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
#endif

#if defined(_useandroid_)
	#include <android/log.h>
#endif

ns1(helpers)

void consolelog(char *tag, char *txt)
{
	#if defined(_usewindows_) || (defined(_useposix_) && !defined(_useandroid_))
		printf("\n%s: %s",tag,txt);
	#endif
	#if defined(_useandroid_)
		__android_log_write(ANDROID_LOG_INFO, tag, txt);
	#endif

}

ns1end