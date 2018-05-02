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

#ifndef AnmarLib_net_iTCPSock_h
#define AnmarLib_net_iTCPSock_h
#include "lib.h"
#include "general.h"

#if defined(_usewindows_)
#include "winsock2.h"
#endif

#if defined(_useposix_)
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>
#include <fcntl.h>
#include <unistd.h>
#endif

namespace ahclib {
	namespace net {
		// Only Connect is Blocking
		class iTCPSock { // TCP Socket Wrapper Class by Anmar Hassan uses Multiplexing for Events on Synchronous sockets
            bool RemoteIPSet;
            bool RemotePortSet;
            bool LocalIPSet;
            bool LocalPortSet;

            static iTCPSock *allsocks[200];
            libroot::general::Event SendBlockEvent;

            static fd_set fd;


            static int BuildFDList();
            bool IsConnecting;
            bool IsListening;

		public:
            bool NoEvents;
            int sock;
            sockaddr_in sin,local;
            int MAX_SEND_SIZ;
            static int sockscount;
            DWORD SocketID;  // User defined long data (custom use)
            DWORD UserInfo2; // Any user defined info (long) (custom use)
            void *UserPointer1; // User pointer 1 (custom use)
            void *UserPointer2; // User pointer 1 (custom use)

            static bool AllowEvent_GlobalDataArrival; // Set TRUE to fire global handler
            bool AllowEvent_DataArrival; // Set TRUE to fire per object handler
            void (*DataArrival)(iTCPSock *, unsigned char *, int);
            void (*Closed) (iTCPSock *);
            void (*ConnectionRequest) (iTCPSock *, int, char *, int);
            bool IsUsed;
            static void (*GlobalDataArrival)(iTCPSock *, unsigned char *, int);
            static bool eventsstarted;
            DWORD TotalBytesRead;
            DWORD TotalBytesWritten;
            DWORD ThisChunkBytesRead;
            DWORD ThisChunkBytesWritten;
            unsigned char buf[32768];

            iTCPSock(void);
            iTCPSock(DWORD Socket_ID);
            ~iTCPSock();

            void clearsockscount();
            // THREAD
            static void* EVENTS(void *ptr);
            DWORD thand;
            DWORD tid;

            void StartEvents();
            // THREAD END

            bool SetRemoteHost(char *NameorIP);
            bool SetLocalHost(char *NameorIP);
            void SetRemotePort(u_short Port);
            void SetLocalPort(u_short Port);
            bool SetRemoteEndPoint(char *NameorIP, u_short Port);
            bool SetLocalEndPoint(char *NameorIP, u_short Port);
            bool Bind();
            bool Listen(int MaxConnections=SOMAXCONN);
            bool Accept(int *ConnectedSock,char **RemoteIP,int *RemotePort);
            void AquireAcceptedSocket(int ConnectedSocket, char *IP, int Port);
            iTCPSock *AcceptAndCreateClass();
            bool Connect();
            bool ConnectAsAsync();
            bool _connecttimeout(int fd,struct sockaddr *remote,int addrsiz,struct timeval *timeout);
            bool ConnectwithTimeOut(long secondstimeout);
            bool BindAsync();
            bool ListenAsync(int MaxConnections=SOMAXCONN);
            int _Send(char *buf, int Siz);
            bool Send(char *buf,long siz);
            int SendText(char *text);
            int Recv(char *buf, int Siz);
            void Close();
		};
	}
}
//iTCPSock *iTCPSock::allsocks[100];
//int iTCPSock::sockscount=0;
//fd_set iTCPSock::fd;
//void (*(iTCPSock::GlobalDataArrival))(iTCPSock *, unsigned char *, int)=0;
//bool iTCPSock::AllowEvent_GlobalDataArrival=FALSE;
//bool iTCPSock::eventsstarted=FALSE;

// Written by Anmar Hassan, July 2008


// Typical Dataarrival event handler
// To set up the event:
// iTCPSock a;
// a.GlobalDataArrival=&GlobalDA; // so as with the local event handlers
// Note: if GlocalDataArrival is NULL the class will not callback the event.

//void GlobalDA(iTCPSock *ts, unsigned char *buf, int len)
//{buf[len]=0;
//printf("\nEVENT GLOBAL DATA ARRIVAL SockID=%d, Data=%s",ts->SocketID,buf);
//sprintf((char *)buf,"This is my Reply From Global for Socket %d",ts->SocketID);
//ts->Send((char *)buf,strlen((char*)buf));
//
//}

#endif
