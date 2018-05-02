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

#ifndef _AnmarLib_sync_H_
#define _AnmarLib_sync_H_

#include "lib.h"
ns1(sync)

#if defined(_useposix_)
	#include "pthread.h"
	#include "time.h"
#endif

#if defined(_usewindows)
#error Not implemented on windows.
#endif
		class Mutex
		{

		public:
			#if defined(_useposix_)
			pthread_mutex_t mutex;
			#endif
			#if defined(_usewindows_)
				HANDLE mutex;
			#endif
			Mutex();
			void Wait();
			void Release();
			~Mutex();
		};

		class Event
		{
			bool state;
			bool AutoReset;
			Mutex mymutex;
			#if defined(_useposix_)
			pthread_cond_t cond;
			#endif
			#if defined(_usewindows_)
			HANDLE _event;
			#endif
		public:
			Event();
			Event(bool isAutoReset);
			~Event();

			void Set();
			void Reset();
			#if defined(_useposix_)
			void InternalWait(timespec *waituntiltime);
			#endif
			void Wait(unsigned int timeoutms);
			void Wait();
		};

ns1end

#endif