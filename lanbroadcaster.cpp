#include "lib.h"
#include "lanbroadcaster.h"
#include "asocket.h"
#include "thread.h"
#include "sync.h"
#include "asocket.h"
#include "general.h"
#if defined(_useposix_)
	#include <string.h>
#endif

ns1(net)

	void lanbroadcaster::bthreadstatic(void *obj)
	{
		if (obj==NULL) return;
		lanbroadcaster *me=(lanbroadcaster *)obj;
		me->bthread();
	}

	void lanbroadcaster::bthread()
	{
		while (bthreadrun)
		{
			bevent->Wait();			// This is here just to pause thread
				
			bsocket->send(broadcastdata,remoteip,remoteport);
			
			libroot::threading::thread::sleep(intervalms);
		}
	}

	void lanbroadcaster::destroy()
	{			
		bthreadrun=false;
		broadcastthread->Join();
		broadcastthread=NULL;
		bevent=NULL;
		broadcastdata.freebuf();
	}

	lanbroadcaster::lanbroadcaster()
	{			
		init();			
	}

	lanbroadcaster::lanbroadcaster(char *data, long size, char *broadcastaddress, int remoteport)
	{
		init();
		updatebroadcastdata(data,size);
		setremotebroadcastendpoint(broadcastaddress, remoteport);
	}

	lanbroadcaster::lanbroadcaster(char *data, long size, long _intervalms, char *broadcastaddress, int remoteport)
	{
		init();
		updatebroadcastdata(data,size);
		setinterval(_intervalms);
		setremotebroadcastendpoint(broadcastaddress, remoteport);
	}

	lanbroadcaster::lanbroadcaster(libroot::general::smartbuf &data, char *broadcastaddress, int remoteport)
	{
		init();
		updatebroadcastdata(data);
		setremotebroadcastendpoint(broadcastaddress, remoteport);
	}

	lanbroadcaster::lanbroadcaster(libroot::general::smartbuf &data, long _intervalms, char *broadcastaddress, int remoteport)
	{
		init();
		updatebroadcastdata(data);
		setinterval(_intervalms);
		setremotebroadcastendpoint(broadcastaddress, remoteport);
	}
	void lanbroadcaster::setinterval(long milliseconds)
	{
		intervalms=milliseconds;
	}		
	void lanbroadcaster::setremotebroadcastendpoint(char *broadcastaddress, int port)
	{
		strcpy(remoteip, broadcastaddress);
		remoteport=port;
	}
	void lanbroadcaster::init()
	{
		intervalms=1000;
		bthreadrun=false;
		bsocket=new libroot::net::asocket(libroot::net::asocket::SocketTypes_UDP,this);
		bsocket->enablebroadcast();
		broadcastthread=new libroot::threading::thread(this,bthreadstatic);
		bevent=new libroot::sync::Event(false);
		bevent->Reset();
		bthreadrun=true;
		broadcastthread->Start();

			
	}

	lanbroadcaster::~lanbroadcaster()
	{
		destroy();
	}

	void lanbroadcaster::updatebroadcastdata(libroot::general::smartbuf &data)
	{
		broadcastdata.setbuffer((char *)data.buffer, data.len);
	}

	void lanbroadcaster::updatebroadcastdata(char *data, long size)
	{
		broadcastdata.setbuffer(data,size);
	}
		
	void lanbroadcaster::startbroadcasting()
	{
		bevent->Set();
	}

	void lanbroadcaster::stopbroadcasting()
	{
		bevent->Reset();
	}

ns1end