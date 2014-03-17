#include "thread.h"

ns1(threading)

#if defined(_useposix_)
#include "pthread.h"
#include <unistd.h>

	void *thread::threadstatic(void *This)
	{
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
       // int oldcanceltype=PTHREAD_CANCEL_DEFERRED;
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
        
		thread *me=(thread *)This;
		if (me->targetthreadfunction!=NULL)
			me->targetthreadfunction(me->userobject);
		return NULL;
	}
	thread::thread()
	{
		joined=FALSE;
	}
	thread::thread(void *usrobj)
	{
		joined=FALSE;
		setuserobject(usrobj);
	}
	thread::~thread()
	{
		if (joined==FALSE) Join();
	}
	thread::thread(void (*_targetthreadfunction)(void *Obj))
	{
		targetthreadfunction=_targetthreadfunction;
		joined=FALSE;
	}
	thread::thread(void *usrobj, void (*_targetthreadfunction)(void *Obj))
	{
		targetthreadfunction=_targetthreadfunction;
		joined=FALSE;
		setuserobject(usrobj);
	}
	void thread::sleep(long milliseconds)
	{
		usleep(milliseconds*1000);
	}
	void thread::setuserobject(void *usrobj)
	{
		userobject=usrobj;
	}
	void thread::Start()
	{
		if (this->targetthreadfunction!=NULL)
		int iret1 = pthread_create( &mthread, NULL, threadstatic, this);
	}
	void thread::Join()
	{
		pthread_join(mthread, NULL);
		joined=TRUE;
	}
    void thread::Cancel()
    {
        pthread_kill(mthread,233);
    }
#endif

#if defined(_usewindows_) && !defined(_WIN32_WINNT_WIN8)
#include "windows.h"
#endif
#if defined(_usewindows_) && defined(_WIN32_WINNT_WIN8)
#include "Processthreadsapi.h"
#endif
#if defined(_usewindows_)
	void *thread::threadstatic(void *This)
	{
		thread *me=(thread *)This;
		if (me->targetthreadfunction!=NULL)
			me->targetthreadfunction(me->userobject);
		return NULL;
	}
	thread::thread()
	{
		joined=FALSE;
	}
	thread::thread(void *usrobj)
	{
		joined=FALSE;
		setuserobject(usrobj);
	}
	thread::~thread()
	{
		if (joined==FALSE) Join();
	}
	thread::thread(void (*_targetthreadfunction)(void *Obj))
	{
		targetthreadfunction=_targetthreadfunction;
		joined=FALSE;
	}
	thread::thread(void *usrobj, void (*_targetthreadfunction)(void *Obj))
	{
		targetthreadfunction=_targetthreadfunction;
		joined=FALSE;
		setuserobject(usrobj);
	}
	void thread::setuserobject(void *usrobj)
	{
		userobject=usrobj;
	}
	void thread::sleep(long milliseconds)
	{
		Sleep(milliseconds);
	}
	void thread::Start()
	{
		DWORD *par=(DWORD *)this;
		DWORD tid=0;
		if (this->targetthreadfunction!=NULL)
			mthread=CreateThread(NULL,NULL,(LPTHREAD_START_ROUTINE)&threadstatic,par,NULL,&tid);
	}
	void thread::Join()
	{
		WaitForSingleObject(mthread, INFINITE);
		
		joined=TRUE;
	}
    void thread:Cancel()
    {
        
    }
#endif

ns1end