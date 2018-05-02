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