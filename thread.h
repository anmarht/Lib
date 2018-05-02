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

#ifndef _AnmarLib_thread_H_
#define _AnmarLib_thread_H_

#include "lib.h"

#if defined(_useposix_)
#include "pthread.h"
#endif
#if defined(_usewindows_) && !defined(_WIN32_WINNT_WIN8)
#include "windows.h"
#endif
#if defined(_usewindows_) && defined(_WIN32_WINNT_WIN8)
#include "windows.h"
#include "Processthreadsapi.h"
#endif

ns1(threading)

	class thread
	{
		bool joined;
		#if defined(_useposix_)
		pthread_t mthread;
		#endif
		#if defined(_usewindows_)
		HANDLE mthread;
		#endif

		void *userobject;

		static void* threadstatic(void *This);
	public:
		static void sleep(long milliseconds);

		void (*targetthreadfunction)(void *userobject);
		thread();
		thread(void *usrobj);
		void setuserobject(void *usrobj);
		thread(void (*_targetthreadfunction)(void *Obj));
		thread(void *usrobj,void (*_targetthreadfunction)(void *Obj));
		~thread();
		void Start();
		void Join();
        void Cancel();
	};

ns1end

#endif