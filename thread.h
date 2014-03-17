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