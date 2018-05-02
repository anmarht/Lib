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
#include "asocket.h"
#if defined(_usewindows_)
	#include <winsock2.h>
	#include <ws2tcpip.h>
#endif
#if defined(_useposix_)
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <string.h>
	#include <errno.h>
	#include <unistd.h>
#endif

#if defined(_useposix_)
	#define SOCKET int
	#define INVALID_SOCKET -1
#endif
#include "netgeneral.h"
#include "general.h"
#include "dns.h"
#include "thread.h"
#include "debugtools.h"
#include "sync.h"

ns1(net)

	bool asocket::initsocket(int sockettype)					// from enum SocketTypes
	{
		userobject=0;
		_sockettype=sockettype;
		receivingthreadrun=false;
		listeningthreadrun=false;
		OnDisconnect=0;
		OnReceive=0;
		OnConnectionAccepted=0;
		sendevent=new libroot::sync::Event(true);
		sendevent->Set();
		maxsendbuffersize=32768;
		memset((void *)&localendpoint,0,sizeof(localendpoint));
		memset((void *)&remoteendpoint,0,sizeof(remoteendpoint));
		#if defined(_usewindows_)
			int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
			if ( iResult != NO_ERROR ) {_print("Error at WSAStartup()\n");return false;}
		#endif
		switch (sockettype)
		{
			case SocketTypes_TCP:
				syssocket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
			break;
			case SocketTypes_UDP:
				syssocket = socket( AF_INET, SOCK_DGRAM, IPPROTO_UDP );
			break;
			default:
				return false;
			break;
		}
		if ( syssocket == INVALID_SOCKET ) 
		{
			#if defined(_usewindows_)
				_print( "\nError at socket(): %ld\n", WSAGetLastError() );WSACleanup(); 
			#endif
			#if defined(_useposix_)
				_print( "\nError creating socket: %s", strerror(errno));
			#endif
			return false;
		}
		if (_sockettype==_UDP) startreceivingthread();
		return true;
	}

	void asocket::setuserobject(void *usrobj)
	{
		userobject=usrobj;
	}

	void *asocket::getuserobject()
	{
		return userobject;
	}

	asocket::asocket(int sockettype)
	{
		initsocket(sockettype);
	}
	asocket::asocket(int sockettype, void *user_object)
	{
		initsocket(sockettype);
		userobject=user_object;
	}
	asocket::~asocket()
	{
		stoplisteningthread();
		stopreceivingthread();
	}

	asocket::errors asocket::setremoteendpoint(char *remotehost, long remoteport)
	{
		char *IPv4;
		if (!libroot::net::IsIPv4(remotehost))
		{
			IPv4=libroot::net::getaddressinfoOneIPv4(remotehost);
			if (IPv4==NULL)
			{
				libroot::net::DNS dns;
				dns.SetTimeout(500);
				dns.SetTotalRetries(20);
				IPv4=dns.QuickQueryHostIPv4(remotehost);
				if (IPv4==NULL)
				{
					return could_not_get_ip_from_domain;
				}

			}

		} else IPv4=remotehost;

		remoteendpoint.sin_family = AF_INET;
		remoteendpoint.sin_addr.s_addr = inet_addr(IPv4);
		remoteendpoint.sin_port = htons(remoteport);	
		return success;
	}

	void asocket::setlocalendpoint(char *localIPv4, long localport)
	{
		localendpoint.sin_family = AF_INET;
		localendpoint.sin_addr.s_addr = inet_addr(localIPv4);
		localendpoint.sin_port = htons(localport);		
	}

	asocket::errors asocket::connect()
	{
		int rv=::connect(syssocket, (sockaddr *) &remoteendpoint, sizeof(remoteendpoint));
		if (rv<0)
		{
			return error_connecting;
		}
		IsConnected=true;

		startreceivingthread();
		return success;
	}

	asocket::errors asocket::listen()
	{
		int rv=bind(syssocket, (struct sockaddr *) &localendpoint, sizeof(localendpoint));
		if (rv<0)
		{
			return error_bind;
		}
		if (_sockettype==_TCP)
		{
			if (::listen(syssocket,SOMAXCONN)!=0)
				return error_listen;
			startlisteningthread();
		}
		if (_sockettype==_UDP)
		{
			startreceivingthread();
		}
		
		return success;
	}

	void asocket::startlisteningthread()
	{
		if (listeningthreadrun) return;
		listeningthreadrun=true;
		listenthread=new libroot::threading::thread(this,listeningthreadstatic);
		listenthread->Start();
	}
	void asocket::stoplisteningthread()
	{
		if (!listeningthreadrun) return;
		listeningthreadrun=false;
		listenthread->Join();
	}

	void asocket::listeningthreadstatic(void *obj)
	{
		if (obj!=NULL) 
		{
			asocket *me=(asocket *)obj;
			me->listeningthread();
		}
		else _print("\nlisteningthreadstatic couldn't call object's thread, object value is NULL.");
	}
	
	void asocket::useacceptedsocket(SOCKET newsyssocketid, char *remoteip, int remoteport)
	{
		syssocket=newsyssocketid;
		setremoteendpoint(remoteip,remoteport);
		startreceivingthread();
	}

	void asocket::rejectacceptedsocket(SOCKET newsyssocketid)
	{
		stopreceivingthread();
		#if defined(_usewindows_)
			closesocket(newsyssocketid);
		#endif
		#if defined(_useposix_)
			::close(newsyssocketid);
		#endif		
	}

	void asocket::listeningthread()
	{
		sockaddr_in sai;
		int sailen=sizeof(sai);
		while (listeningthreadrun)
		{	
			#if defined(_usewindows_)
				SOCKET rv=::accept(syssocket, (sockaddr *)&sai, &sailen);
			#endif
			#if defined(_useposix_)
				socklen_t flen=(socklen_t)sailen;
				SOCKET rv=::accept(syssocket, (sockaddr *)&sai, (socklen_t *)&flen);
			#endif
			
			if (rv>=0)
			{
				char *remoteip=inet_ntoa((in_addr)sai.sin_addr);
				int remoteport=ntohs(sai.sin_port);
				
				if (OnConnectionAccepted!=NULL)
				{
					OnConnectionAccepted(this,rv,remoteip,remoteport);
				}
				else
					rejectacceptedsocket(rv);

			}
			else
            {
                _print("\nError in accept, in a listening asocket. rv=%d. Listening thread terminated.",rv);
                listeningthreadrun=false;
            }

		}
	}
	void asocket::setOnReceive(void (*pOnReceive)(asocket *, libroot::general::smartbuf &buf, char *, int))
	{
		OnReceive=pOnReceive;
	}
	void asocket::setOnDisconnect(void (*pOnDisconnect) (asocket *))
	{
		OnDisconnect=pOnDisconnect;
	}
	void asocket::setOnConnectionAccepted(void (*pOnConnectionAccepted) (asocket *, SOCKET, char *, int))
	{
		OnConnectionAccepted=pOnConnectionAccepted;
	}
	
	void asocket::setcallbacks(void (*pOnReceive)(asocket *, libroot::general::smartbuf &buf, char *, int), void (*pOnDisconnect) (asocket *), void (*pOnConnectionAccepted) (asocket *, SOCKET, char *, int))
	{
		setOnReceive(pOnReceive);
		setOnDisconnect(pOnDisconnect);
		setOnConnectionAccepted(pOnConnectionAccepted);
	}

	void asocket::startreceivingthread()
	{
		if (receivingthreadrun) return;
		receivingthreadrun=true;
		recvthread=new libroot::threading::thread(this,receivingthreadstatic);
		recvthread->Start();
	}
	void asocket::stopreceivingthread()
	{
		if (!receivingthreadrun) return;
		receivingthreadrun=false;
		recvthread->Join();
	}

	void asocket::receivingthreadstatic(void *obj)
	{
		if (obj!=NULL) 
		{
			asocket *me=(asocket *)obj;
			me->recevingthread();
		}
		else _print("\nreceivingthreadstatic couldn't call object's thread, object value is NULL.");
	}

	void asocket::recevingthread()
	{
		libroot::general::smartbuf buf,dispatched;
		buf.resize(32768);
		socklen_t flen;
		sockaddr_in rendpoint;
		memset((void *)&rendpoint,0,sizeof(rendpoint));
		while (receivingthreadrun)
		{
			int rv=-1;
			int fromlen=0;


			switch (_sockettype)
			{
				case SocketTypes_UDP:
					#if defined(_usewindows_)
						fromlen=sizeof(rendpoint);
						rv=recvfrom(syssocket,(char *)buf.buffer,buf.len,0,(sockaddr*)&rendpoint,&fromlen);	

					#endif
					#if defined(_useposix_)
						flen=(socklen_t)sizeof(rendpoint);
						rv=recvfrom(syssocket,(char *)buf.buffer,buf.len,0,(sockaddr*)&rendpoint,&flen);	
					#endif
				break;
				
				case SocketTypes_TCP:
					rv=recv(syssocket,(char *)buf.buffer,buf.len,NULL);
				break;

				default:
					receivingthreadrun=false;
					if (OnDisconnect!=NULL)
					{					
						OnDisconnect(this);
					}
				break;
			}

			if (rv>0)
			{
				if (OnReceive!=NULL)
				{
					char *remoteip=NULL;
					int remoteport=0;
					if (_sockettype==_UDP)
					{
						remoteip=inet_ntoa((in_addr)rendpoint.sin_addr);
						remoteport=ntohs(rendpoint.sin_port);
					}
					dispatched.setbuffer((char *)buf.buffer,rv);
					OnReceive(this,dispatched, remoteip, remoteport);
				}
			}
			else
			{
                if (receivingthreadrun)
                {
                    receivingthreadrun=false;
                  //  close();
                    if (OnDisconnect!=NULL)
                    {
                        OnDisconnect(this);
                    }
                }
			}


		}
        

	}

	void asocket::close()
	{
		shutdown(syssocket, SHUT_RDWR);
		#if defined(_usewindows_)
			closesocket(syssocket);
		#endif
		#if defined(_useposix_)
			::close(syssocket);
		#endif
        stopreceivingthread();
	}

    void asocket::forceshutdown()
    {
        close();
        return;
        shutdown(syssocket, SHUT_RDWR);
        
        if (receivingthreadrun)
        {
            receivingthreadrun=false;
            recvthread->Cancel();
        }
        
        if (listeningthreadrun)
        {
            listeningthreadrun=false;
            listenthread->Cancel();
        }
        close();
    }

	bool asocket::send(void *buf, long size, char *remoteip, int remoteport)
	{
		sockaddr_in remotetmp, *remoteendselector;
		if (_sockettype==_UDP)
		{
			if (remoteip==NULL) remoteendselector=&remoteendpoint;
			else 
			{
				remoteendselector=&remotetmp;
				memset((void *)&remotetmp,0,sizeof(remotetmp));
				remotetmp.sin_family = AF_INET;
				remotetmp.sin_addr.s_addr = inet_addr(remoteip);
				remotetmp.sin_port = htons(remoteport);	
			}
		}

		sendevent->Wait();

		if (size>maxsendbuffersize)
		{
			int chunksize=maxsendbuffersize;
			long sentsize=0;int rv;
			while (sentsize<size)
			{
				
				if ((size-sentsize)>maxsendbuffersize)
					chunksize=maxsendbuffersize;
				else
					chunksize=chunksize=size-sentsize;
				if (_sockettype==_TCP)
					rv=::send(syssocket,(char *)buf+sentsize,chunksize,NULL);
				if (_sockettype==_UDP)
					rv=::sendto(syssocket,(char *)buf+sentsize,chunksize,NULL,(sockaddr *)remoteendselector ,sizeof(remoteendpoint));
				if (rv<0) 
				{
					_print("\nSocket_Error Sending chunk...");
					sendevent->Set();
					return FALSE;
				} else sentsize+=rv;
			} 
		}
		else 
		{
			int rv=0;
			if (_sockettype==_TCP)
				rv=::send(syssocket,(char *)buf,size,NULL);
			if (_sockettype==_UDP)
				rv=::sendto(syssocket,(char *)buf,size,NULL,(sockaddr *)remoteendselector ,sizeof(remoteendpoint));
		}
		
		sendevent->Set();

		return TRUE;		
	}

	void asocket::enablebroadcast()
	{
		int broadcastPermission = 1;
		setsockopt(syssocket, SOL_SOCKET, SO_BROADCAST, (char *) &broadcastPermission, sizeof(broadcastPermission));
	}

	bool asocket::send(libroot::general::smartbuf &buf, char *remoteip, int remoteport)
	{
		return send(buf.buffer,buf.len, remoteip, remoteport);
	}

	bool asocket::write(char *txt, char *remoteip, int remoteport)
	{
		return send(txt,strlen(txt), remoteip, remoteport);
	}

	bool asocket::writeln(char *txt, char *remoteip, int remoteport)
	{
		libroot::general::smartbuf b;
		b.push(txt,strlen(txt));
		b.push((char *)"\r\n",2);
		return send(b, remoteip, remoteport);
	}

ns1end