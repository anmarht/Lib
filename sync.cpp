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

#include "sync.h"

ns1(sync)


#if defined(_useposix_)
	#include "pthread.h"
	#include "errno.h"
#ifdef _useios_
    #include "sys/time.h"
#else
	#include "time.h"
#endif
	// Mutex Class Start
	Mutex::Mutex()
	{
		pthread_mutex_init(&mutex, NULL);
	}
	void Mutex::Wait()
	{
			pthread_mutex_lock(&mutex);
	}
	void Mutex::Release()
	{
		pthread_mutex_unlock(&mutex);
	}
	Mutex::~Mutex()
	{
		pthread_mutex_destroy(&mutex);
	}

	// Mutex Class End
#endif
#if defined(_usewindows_)
	// Mutex Class Start
	Mutex::Mutex()
	{
		mutex = CreateMutex( 
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);  
	}
	void Mutex::Wait()
	{
		 unsigned long dwWaitResult = WaitForSingleObject( 
            mutex,    // handle to mutex
            INFINITE);  // no time-out interval
	}
	void Mutex::Release()
	{
		 ReleaseMutex(mutex);
	}
	Mutex::~Mutex()
	{
		CloseHandle(mutex);
	}
#endif
#if defined(_useposix_)
	// Event Class Start
	Event::Event()
	{
		state=FALSE;
		AutoReset=FALSE;
		pthread_cond_init(&cond, NULL);
	}
	Event::Event(bool isAutoReset)
	{
		state=FALSE;
		AutoReset=isAutoReset;
		pthread_cond_init(&cond, NULL);
		Set();
	}
	Event::~Event()
	{
		pthread_cond_destroy(&cond);
	}

	void Event::Set()
	{
		mymutex.Wait();
		if (state==FALSE)
		{
			state=TRUE;
			pthread_cond_broadcast(&cond);
		}
		mymutex.Release();
	}
	void Event::Reset()
	{
		mymutex.Wait();
		state=FALSE;
		mymutex.Release();
	}
	void Event::InternalWait(timespec *waituntiltime)
	{
		int rv=-1;
		if (state) return;
		mymutex.Wait();
		do
		{
			if (waituntiltime!=NULL)
			rv = pthread_cond_timedwait(&cond, &mymutex.mutex, waituntiltime);
			else
			rv = pthread_cond_wait(&cond, &mymutex.mutex);

		} while (!state && rv != ETIMEDOUT);
		if (AutoReset) Reset();

		mymutex.Release();

	}


	void Event::Wait(unsigned int timeoutms)
	{
		timespec waituntiltime;
		if (time!=NULL)
		{
			timespec now, timeout;
            
#ifdef _useios_
            struct timeval machnow;
            int rv = gettimeofday(&machnow, NULL);
            if (rv)
            {
                now.tv_sec=0;
                now.tv_nsec=0;
            }
            else
            {
                now.tv_sec  = machnow.tv_sec;
                now.tv_nsec = machnow.tv_usec * 1000;
            }
            
#else
            clock_gettime(CLOCK_REALTIME, &now);
            
#endif
  
			timeout.tv_sec = timeoutms / 1000; // Seconds
			timeout.tv_nsec = (timeoutms % 1000) * 1000000L; // Nanoseconds

			waituntiltime.tv_sec = now.tv_sec + timeout.tv_sec;
			waituntiltime.tv_nsec = now.tv_nsec + timeout.tv_nsec;
			if (waituntiltime.tv_nsec >= 1000000000L)
			{
				waituntiltime.tv_nsec -= 1000000000L;
				waituntiltime.tv_sec++;
			}
		}
		InternalWait(&waituntiltime);

	}
	void Event::Wait()
	{
		InternalWait(NULL);

	}
	// Event Class End

#endif

#if defined(_usewindows_)
		// Event Class Start
	Event::Event()
	{
		state=FALSE;
		AutoReset=FALSE;
		_event=CreateEvent(NULL,TRUE,TRUE,NULL);
	}
	Event::Event(bool isAutoReset)
	{
		state=FALSE;
		AutoReset=isAutoReset;
		_event=CreateEvent(NULL,FALSE,TRUE,NULL);
	}
	Event::~Event()
	{
		CloseHandle(_event);
	}

	void Event::Set()
	{		
		SetEvent(_event);
		state=true;
	}
	void Event::Reset()
	{
		ResetEvent(_event);
		state=false;
	}

	void Event::Wait(unsigned int timeoutms)
	{
		WaitForSingleObject(_event,timeoutms);
	}
	void Event::Wait()
	{
		WaitForSingleObject(_event,INFINITE);
		if (!AutoReset) Set();
	}
	// Event Class End

#endif


ns1end