#pragma once

#ifndef _AnmarLib_lanbroadcaster_H_
#define _AnmarLib_lanbroadcaster_H_

#include "lib.h"
#include "asocket.h"

#include "thread.h"
#include "sync.h"
#include "asocket.h"
#include "general.h"

ns1(net)

	class lanbroadcaster
	{
		libroot::general::smartbuf broadcastdata;
		libroot::threading::thread *broadcastthread;
		bool bthreadrun;
		libroot::sync::Event *bevent;
		long intervalms;
		libroot::net::asocket *bsocket;	

		char remoteip[34];
		int remoteport;

		static void bthreadstatic(void *obj);

		void bthread();

		void destroy();

	public:

		lanbroadcaster();

		lanbroadcaster(char *data, long size, char *broadcastaddress, int remoteport);

		lanbroadcaster(char *data, long size, long _intervalms, char *broadcastaddress, int remoteport);

		lanbroadcaster(libroot::general::smartbuf &data, char *broadcastaddress, int remoteport);

		lanbroadcaster(libroot::general::smartbuf &data, long _intervalms, char *broadcastaddress, int remoteport);
		void setinterval(long milliseconds);
		void setremotebroadcastendpoint(char *broadcastaddress, int port);
		void init();
		~lanbroadcaster();

		void updatebroadcastdata(libroot::general::smartbuf &data);

		void updatebroadcastdata(char *data, long size);
		
		void startbroadcasting();

		void stopbroadcasting();

	};

ns1end

#endif