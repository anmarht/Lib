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

#ifndef _AnmarLib_debugtools_H_
#define _AnmarLib_debugtools_H_

#include "lib.h"
#define _print libroot::print
#if defined(_debug)
#define _dprint _print
#else
#define _dprint libroot::donothing
#endif

mainns
	void setprinttag(char *ptag);
	void print(const char * _Format, ...);
	void donothing(const char * _Format, ...);
nsend

#endif