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

#ifndef _AnmarLib_Asocket_H_
#define _AnmarLib_Asocket_H_

#include "lib.h"
#include "thread.h"
#include "general.h"
#include "sync.h"
#if defined(_usewindows_)
	#include <winsock2.h>
	#include <ws2tcpip.h>
#endif
#if defined(_useposix_)
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
#endif
#if defined(_useposix_)
	#define SOCKET int
	#define INVALID_SOCKET -1
#endif

ns1(net)

	class asocket
	{
		#if defined(_usewindows_)
			WSADATA wsaData;
		#endif
		SOCKET syssocket;
		sockaddr_in remoteendpoint, localendpoint;

		#if defined(_usewindows_)
			#define SocketTypes_TCP SocketTypes::_TCP
			#define SocketTypes_UDP SocketTypes::_UDP
		#endif
		#if defined(_useposix_)
			#define SocketTypes_TCP _TCP
			#define SocketTypes_UDP _UDP
		#endif

		bool initsocket(int sockettype);

		bool receivingthreadrun;
		bool listeningthreadrun;
		libroot::threading::thread *listenthread;
		libroot::threading::thread *recvthread;
	
		libroot::sync::Event *sendevent;
		int _sockettype;

		void startlisteningthread();
		void stoplisteningthread();
		void startreceivingthread();
		void stopreceivingthread();
		static void listeningthreadstatic(void *obj);
		static void receivingthreadstatic(void *obj);
		void recevingthread();
		void listeningthread();

	public:
		int maxsendbuffersize;
		void *userobject;

		enum SocketTypes
		{
			_TCP=					0x01,
			_UDP=					0x02,
		};
		bool IsConnected;
		enum errors
		{
			success=0,
			could_not_get_ip_from_domain=10,
			error_connecting=20,
			error_bind=30,
			error_listen=40,
		};

		// Callbacks
		void (*OnReceive)(asocket *, libroot::general::smartbuf &buf, char *, int);
		void (*OnDisconnect) (asocket *);
		void (*OnConnectionAccepted) (asocket *, SOCKET, char *, int);

		void setOnReceive(void (*pOnReceive)(asocket *, libroot::general::smartbuf &buf, char *, int));
		void setOnDisconnect(void (*pOnDisconnect) (asocket *));
		void setOnConnectionAccepted(void (*pOnConnectionAccepted) (asocket *, SOCKET, char *, int));

		void enablebroadcast();

		void setcallbacks(void (*pOnReceive)(asocket *, libroot::general::smartbuf &buf, char *, int), void (*pOnDisconnect) (asocket *), void (*pOnConnectionAccepted) (asocket *, SOCKET, char *, int));

		void setuserobject(void *usrobj);

		void *getuserobject();

		// sockettype is one of the values defined by SocketTypes enum.
		asocket(int sockettype);

		asocket(int sockettype, void *user_object);

		~asocket();	

		errors setremoteendpoint(char *remotehost, long remoteport);
		
		void setlocalendpoint(char *localIPv4, long localport);
	
		// Attempts to connect, expects remote endpoint to be set correctly.
		errors connect();

		errors listen();

		void useacceptedsocket(SOCKET newsyssocketid, char *remoteip, int remoteport);

		void rejectacceptedsocket(SOCKET newsyssocketid);

		void close();
        
        void forceshutdown();

		bool send(void *buf, long size, char *remoteip=NULL, int remoteport=0);

		bool send(libroot::general::smartbuf &buf, char *remoteip=NULL, int remoteport=0);

		bool write(char *txt, char *remoteip=NULL, int remoteport=0);

		bool writeln(char *txt, char *remoteip=NULL, int remoteport=0);

	};

ns1end

#endif