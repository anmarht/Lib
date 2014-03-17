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