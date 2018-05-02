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

#include "lib.h"
#include "general.h"
#include "TCPSock.h"

#if defined (_usewindows_)

#endif

#if defined (_useposix_)
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

#include "string.h"

namespace ahclib {
	namespace net {

		int iTCPSock::BuildFDList()
		{int i,cnt;
		 FD_ZERO(&fd);
	//	 fd.fd_count=0;


		 cnt=0;
	//	 printf("\nSocksCount=%d",sockscount);
		 for (i=0;i<sockscount;i++)
		 {if (allsocks[i]->IsUsed==TRUE) {cnt++;FD_SET(allsocks[i]->sock,&fd);}
		 }
		 return cnt;
		 
		}



		iTCPSock::iTCPSock(void) {
			  MAX_SEND_SIZ=32767;
			  RemoteIPSet=FALSE;
			  RemotePortSet=FALSE;
			  AllowEvent_DataArrival=FALSE;
			  DataArrival=NULL;
			  Closed=NULL;
			  IsUsed=FALSE;
			  IsConnecting=IsListening=FALSE;
			//  printf("\nSocksCount=%d",sockscount);
			  allsocks[sockscount]=this;
			  
			  sockscount++;
			  TotalBytesRead=TotalBytesWritten=ThisChunkBytesRead=ThisChunkBytesWritten=0;		




            // SendBlockEvent=CreateEvent(NULL,TRUE,TRUE,L"");
			SendBlockEvent.set();
            NoEvents=FALSE;
            StartEvents();
		}
		iTCPSock::iTCPSock(DWORD Socket_ID)
		 {
			SocketID=Socket_ID;

			iTCPSock();
		 }
		iTCPSock::~iTCPSock(void)
		{
			long i,j;
			for (i=0;i<sockscount;i++)
			{
				if (allsocks[i]==this) {
					if (i==sockscount-1) {
						sockscount--;
						return;
					}
					if (i<sockscount-1) {
						for (j=i+1;j<sockscount;j++)
						allsocks[j-1]=allsocks[j];
						sockscount--;
						return;
					}

				}

			}
		}
		void iTCPSock::clearsockscount()
		 {
		sockscount=0;
		 }

		bool runthread=FALSE;

		// THREAD
		void *iTCPSock::EVENTS(void *ptr)
		{
		int i,rv,recvrv;
		timeval tv;
		char buf[10];
		eventsstarted=TRUE;
		tv.tv_sec=8;
		tv.tv_usec=0;

		while (runthread)
		{
			//Sleep(500);
			if (BuildFDList()==0) ;//Sleep(200);

			rv=select(1024,&fd,NULL,NULL,&tv);
			//itoa(rv,buf,10);
		 //       NSLog(@"from inside thread RV=%d\n",rv);


			//printf("\nSelect Returned: %d",rv);
				//SOCKET_ERROR = -1
			if (rv==-1) {//Sleep(100);
			//printf("\nSocket error occured rv=%d",rv);
			//printf("\nWSAGetLastError = %d",WSAGetLastError());

			}
			if (rv>0) {
			for (i=0;i<sockscount;i++)
			{if (allsocks[i]->NoEvents==FALSE) {
				if (FD_ISSET(allsocks[i]->sock,&fd)) {

					if (allsocks[i]->IsConnecting==TRUE) {

						if ((allsocks[i]->DataArrival!=NULL)||(GlobalDataArrival!=NULL))
						{
							recvrv=allsocks[i]->Recv((char *)allsocks[i]->buf,sizeof(allsocks[i]->buf));

						}

						if (recvrv<=0) {
								allsocks[i]->IsUsed=FALSE;
								allsocks[i]->Close();
							if (allsocks[i]->Closed!=NULL) allsocks[i]->Closed(allsocks[i]); // Fire event Closed Remotely
							}
						else {
					//	if (allsocks[i]->AllowEvent_DataArrival==TRUE) {
							if (allsocks[i]->DataArrival!=NULL) allsocks[i]->DataArrival(allsocks[i],allsocks[i]->buf,allsocks[i]->ThisChunkBytesRead);
					//	}
					//	if (AllowEvent_GlobalDataArrival==TRUE) {
							if (GlobalDataArrival!=NULL) iTCPSock::GlobalDataArrival(allsocks[i],allsocks[i]->buf,allsocks[i]->ThisChunkBytesRead);
					//	}
						}

					} // IsConnecting

					if (allsocks[i]->IsListening==TRUE) {
						int connsock;
						char *RemIP=NULL;int RemPort;
						if (allsocks[i]->Accept(&connsock,&RemIP,&RemPort)==TRUE) {

						if (allsocks[i]->ConnectionRequest!=NULL) allsocks[i]->ConnectionRequest(allsocks[i],connsock,RemIP,RemPort);
						}
					}

				}
			}
			}

			}
		}


		return NULL;
		}

		libroot::general::Thread *eventsthread;

		void iTCPSock::StartEvents()
		{
			DWORD par;
			if (eventsstarted==TRUE) return;

			eventsthread=new libroot::general::Thread(EVENTS);

			eventsthread->CreateThread();

		}


		bool iTCPSock::SetRemoteHost(char *NameorIP) {
			char OutIP[20];

			RemoteIPSet=FALSE;
			sin.sin_addr.s_addr = inet_addr(NameorIP);
			sin.sin_family = AF_INET;
			RemoteIPSet=TRUE;
			return TRUE;
		}
		bool iTCPSock::SetLocalHost(char *NameorIP) {
			char OutIP[20];
			LocalIPSet=FALSE;
			local.sin_addr.s_addr = inet_addr(NameorIP);	
			 local.sin_family = AF_INET;
			 LocalIPSet=TRUE;
			 return TRUE;
		}
		void iTCPSock::SetRemotePort(u_short Port)
		{
		  sin.sin_port = htons(Port);	
		  RemotePortSet=TRUE;  
		}
		void iTCPSock::SetLocalPort(u_short Port)
		{
		  local.sin_port = htons(Port);	
		  LocalPortSet=TRUE;  
		}
		bool iTCPSock::SetRemoteEndPoint(char *NameorIP, u_short Port)
		{
			if (SetRemoteHost(NameorIP))
			{
				SetRemotePort(Port);
				return TRUE;
			}
			else return FALSE;
		}
		bool iTCPSock::SetLocalEndPoint(char *NameorIP, u_short Port)
		{
			if (SetLocalHost(NameorIP))
			{
				SetLocalPort(Port);
				return TRUE;
			}
			else return FALSE;
		}

		bool iTCPSock::Bind()
		{sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
			IsUsed=TRUE;
		if (bind(sock,(struct sockaddr *) &local,sizeof(local)) == -1) {
			close(sock);

			IsUsed=FALSE;
			return FALSE;
			}
		return TRUE;
		}
	//	bool TCPSock::BindAsync()
	//	{sock = WSASocket(AF_INET,SOCK_STREAM,NULL,NULL,NULL,NULL);
	//		IsUsed=TRUE;
	//	if (bind(sock,(struct sockaddr *) &local,sizeof(local)) == -1) {
	//		closesocket(sock);
	//		IsUsed=FALSE;
	//		return FALSE;
	//		}
	//	return TRUE;
	//	}
		bool iTCPSock::Listen(int MaxConnections)
		{
		if (listen(sock, MaxConnections)==-1)
			return FALSE;
		else
		{
			IsListening=TRUE;IsConnecting=FALSE;
		return TRUE;
		}
		}
		bool iTCPSock::ListenAsync(int MaxConnections)
		{
			if (listen(sock, MaxConnections)==-1)
			return FALSE;
			else
			{
				IsListening=TRUE;IsConnecting=FALSE;
			return TRUE;
			}
		}
		bool iTCPSock::Accept(int *ConnectedSock,char **RemoteIP,int *RemotePort)
		{struct sockaddr_in remoteparty; //INVALID_SOCKET
		 socklen_t rempartyaddrsize;
		rempartyaddrsize=sizeof(remoteparty);
		 *ConnectedSock=accept(sock,(struct sockaddr *) &remoteparty,&rempartyaddrsize);
		 if (*ConnectedSock==-1) return FALSE;
		 else {
		  *RemoteIP=inet_ntoa((in_addr)remoteparty.sin_addr);
		  *RemotePort=ntohs(remoteparty.sin_port);
		  return TRUE;
		 }
		}

		void iTCPSock::AquireAcceptedSocket(int ConnectedSocket, char *IP, int Port)
		{
			sin.sin_family = AF_INET;
			sin.sin_addr.s_addr = inet_addr(IP);
			RemoteIPSet=TRUE;
			sin.sin_port = htons(Port);	
			RemotePortSet=TRUE; 
			sock=ConnectedSocket;
			IsListening=FALSE;IsConnecting=TRUE;
			IsUsed=TRUE;
		}


		iTCPSock *iTCPSock::AcceptAndCreateClass()
		{
		iTCPSock *newTCPSock=new iTCPSock();

		return NULL;
		}

		bool iTCPSock::Connect()
		{int rv;
			if (RemoteIPSet&&RemotePortSet) {
			sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
			IsUsed=TRUE;
			rv=connect(sock,(struct sockaddr *)&sin, sizeof(sin));
			if (rv!=0) return FALSE; else 
			{IsListening=FALSE;IsConnecting=TRUE;
				return TRUE;}
			}
		}
		bool iTCPSock::ConnectAsAsync()
		{	int rv,x;
			if (RemoteIPSet&&RemotePortSet) {
		//	sock = WSASocket(AF_INET,SOCK_STREAM,NULL,NULL,NULL,NULL);
            sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
            x=fcntl(sock,F_GETFL,0);              // Get socket flags
            fcntl(sock,F_SETFL,x | O_NONBLOCK);   // Add non-blocking flag

			IsUsed=TRUE;
			rv=connect(sock,(struct sockaddr *)&sin, sizeof(sin));
			if (rv!=0) return FALSE; else 
			{IsListening=FALSE;IsConnecting=TRUE;
				return TRUE;}
			}
		}

		bool iTCPSock::ConnectwithTimeOut(long secondstimeout)
		{bool rv;
		struct timeval to;
			if (RemoteIPSet&&RemotePortSet) {
			sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
			IsUsed=TRUE;
			to.tv_usec=0;
			to.tv_sec=secondstimeout;
			rv=_connecttimeout(sock,(struct sockaddr *)&sin, sizeof(sin),&to);
			if (rv==FALSE) {IsUsed=FALSE;return FALSE;} else 
			{IsListening=FALSE;IsConnecting=TRUE;
				return TRUE;}
			}
		}
		bool iTCPSock::_connecttimeout(int fd,struct sockaddr *remote,int addrsiz,struct timeval *timeout)
		{
				int saveflags,ret,rv;
				fd_set fd_w;


			//u_long iMode = 1;

            // Set non-blocking: equivalent to fcntl(fd, FIONBIO, &iMode); when iMode is !=0

            int flags = fcntl(fd, F_GETFL, 0);
            fcntl(fd, F_SETFL, flags | O_NONBLOCK);


				//* This will return immediately
			   rv=connect(fd,remote,addrsiz);	
//iMode = 0;
				//fcntl(fd, FIONBIO, &iMode);

            // Set blocking
            flags = fcntl(fd, F_GETFL, 0);
            fcntl(fd, F_SETFL, flags & (!O_NONBLOCK));

				//* return unless the connection was successful or the connect is still in progress.
				FD_ZERO(&fd_w);
				FD_SET(fd,&fd_w);
				rv=select(FD_SETSIZE,NULL,&fd_w,NULL,timeout);
				if(rv<0) return FALSE;
				//* 0 means it timeout out & no fds changed
				if(rv==0) {
						close(fd);
						return FALSE;
				}
				//* Get the return code from the connect
				//int len;
				socklen_t len=sizeof(ret);
				rv=getsockopt(fd,SOL_SOCKET,SO_ERROR,(char *)&ret,&len);
				if(rv<0) return FALSE;

				//* ret=0 means success, otherwise it contains the errno
				if(ret) return FALSE;
		       
				return TRUE;
		}


		int iTCPSock::_Send(char *buf, int Siz) {
			int rv;
			rv=send(sock, buf, Siz, 0);
			if (rv!=-1)
			{ThisChunkBytesWritten=rv;TotalBytesWritten+=rv;} else ThisChunkBytesWritten=0;
			return rv;
		}
		bool iTCPSock::Send(char *buf,long siz)
		{	int rv,chunksiz;
			long sentsiz;
			sentsiz=0;
            SendBlockEvent.wait_infinite();

		//	WaitForSingleObject(SendBlockEvent,INFINITE);
			//ResetEvent(SendBlockEvent);
            SendBlockEvent.reset();

			while ((siz-sentsiz)>MAX_SEND_SIZ) 
			{
				chunksiz=MAX_SEND_SIZ; 
				rv=_Send((char *)buf+sentsiz,chunksiz);//sleep(5);
				if (rv==-1) {//printf("\nSocket_Error Send");
                    //SetEvent(SendBlockEvent);
                    SendBlockEvent.set();
                    return FALSE;} else sentsiz+=rv;
			}
			chunksiz=siz-sentsiz;
			if (chunksiz>0) {
				rv=_Send((char *)buf+sentsiz,chunksiz);
				if (rv==-1) {//printf("\nSocket_Error Send");
                   // SetEvent(SendBlockEvent);
                    SendBlockEvent.set();
                    return FALSE;} else sentsiz+=rv;
			}
			//SetEvent(SendBlockEvent);
            SendBlockEvent.set();
			return TRUE;
		}

		int iTCPSock::SendText(char *text) {
			return Send(text,strlen(text));
		}

		int iTCPSock::Recv(char *buf, int Siz) {
		int rv;
			rv=recv(sock, buf, Siz, 0);
			if (rv!=-1)
			{ThisChunkBytesRead=rv;TotalBytesRead+=rv;} else ThisChunkBytesRead=0;
			return rv;
		}

		void iTCPSock::Close()
		{
		IsUsed=FALSE;
		close(sock);
		}




		//
		iTCPSock *iTCPSock::allsocks[200];
		int iTCPSock::sockscount=0;
		fd_set iTCPSock::fd;
		void (*(iTCPSock::GlobalDataArrival))(iTCPSock *, unsigned char *, int)=0;
		bool iTCPSock::AllowEvent_GlobalDataArrival=FALSE;
		bool iTCPSock::eventsstarted=FALSE;


	}
}
// Written by Anmar Hassan, July 2008


// Typical Dataarrival event handler 
// To set up the event:
// TCPSock a;
// a.GlobalDataArrival=&GlobalDA; // so as with the local event handlers
// Note: if GlocalDataArrival is NULL the class will not callback the event.

//void GlobalDA(TCPSock *ts, unsigned char *buf, int len)
//{buf[len]=0;
//printf("\nEVENT GLOBAL DATA ARRIVAL SockID=%d, Data=%s",ts->SocketID,buf);
//sprintf((char *)buf,"This is my Reply From Global for Socket %d",ts->SocketID);
//ts->Send((char *)buf,strlen((char*)buf));
//
//}
