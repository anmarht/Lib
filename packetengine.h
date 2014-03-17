#pragma once
#ifndef _AnmarLib_packetengine_H_
#define _AnmarLib_packetengine_H_

#include "lib.h"
#include "asocket.h"
#include "netgeneral.h"
#include "general.h"
#include "sync.h"
#include "PacketBase.h"
#include <string.h>
ns1(net)
	template <class PacketClass> // PacketClass is any class inherting from PacketBase
	class packetengine {        
		asocket *ts;
		//libroot::net::packetreceiver pr;

		PacketClass dummy;
	
		char psig[100];
		int psigsize;
		long fixedheadersize;
		long thispacketlength;
		long pos;
		int state;
		libroot::general::smartbuf accumulator;

		static void onreceivestatic(asocket *sock, libroot::general::smartbuf &buf, char *remoteip, int remoteport);
		void onreceive(asocket *sock, libroot::general::smartbuf &buf, char *remoteip, int remoteport);

		static void ondisconnectstatic(asocket *sock);
		void ondisconnect(asocket *sock);
		
		void (*onpacketreceived) (packetengine *, PacketClass &);
		void (*ondisconnected)(packetengine *);

	public:
		libroot::sync::Mutex SyncMutex;
		void *userobject;

		void setuserobject(void *usrobj);
		void *getuserobject();

		void setonpacketreceived(void (*_onpacketreceived) (packetengine *, PacketClass &));
		void setondisconnected(void (*_ondisconnected) (packetengine *));
		void setcallbacks(void (*_onpacketreceived) (PacketClass &), void (*_ondisconnected) ());


		packetengine ();
		~packetengine ();
		void useacceptedsocket(SOCKET connectedsock,char *RIP, int Port);
		bool connectto(char *ServerHost, int Port);
		void close();
        void forceclose();
		void sendpacket(PacketClass *pbc);

	};


	template <class PacketClass> void packetengine<PacketClass>::onreceivestatic(asocket *sock, libroot::general::smartbuf &buf, char *remoteip, int remoteport)
	{
		packetengine *peobj=(packetengine *)sock->getuserobject();
		peobj->onreceive(sock,buf,remoteip,remoteport);
	}

	template <class PacketClass> void packetengine<PacketClass>::onreceive(asocket *sock, libroot::general::smartbuf &buf, char *remoteip, int remoteport)
	{
		// Receive in a buffer,
		// keep accumulating until u reach the size of the fixed header
		// when size of the fixed header is reached, keep accumulating until u reach the size of the full packet, then dispatch.

		accumulator.push((char *)buf.buffer,buf.len);
		if (state==0)
		{
			pos=accumulator.instr(psig,psigsize,0);
			if (pos>=0) state=1;
		}

		if (state==1)
		{
			if ((accumulator.len-pos)>=fixedheadersize)
			{
				// get the full packet length from header
				thispacketlength=dummy.returnpacketsizefromheaderdata((void *)((char *)accumulator.buffer+pos));
				state=2;
			}
		}
		if (state==2)
		{
			if ((accumulator.len-pos)>=thispacketlength)
			{
				// We've got a pure packet, take it out, throw the garbage down to the begining, and keep the data after.

				PacketClass newpacket;
				newpacket.SetPacketFromRawData((void *)((char *)accumulator.buffer+pos),thispacketlength);
			//	libroot::general::smartbuf purepacket;
			//	purepacket.push((char *)accumulator.buffer+pos,thispacketlength);
				accumulator.cropleft(pos+thispacketlength);

				state=0;
				
				if (onpacketreceived!=NULL) onpacketreceived(this, newpacket);
			}

		}

	}

	template <class PacketClass> void packetengine<PacketClass>::ondisconnectstatic(asocket *sock)
	{
		packetengine *peobj=(packetengine *)sock->getuserobject();
		peobj->ondisconnect(sock);
	}

	template <class PacketClass>void packetengine<PacketClass>::ondisconnect(asocket *sock)
	{
		if (ondisconnected!=NULL) ondisconnected(this);
	}

	template <class PacketClass> packetengine<PacketClass>::packetengine () {
		ts=new asocket(asocket::_TCP);
		ts->setOnReceive(&onreceivestatic);
		ts->setOnDisconnect(&ondisconnectstatic);
		ts->setuserobject(this);
		state=0;
		pos=0;
		psigsize=dummy.getpacketsignaturesize();
		strncpy(psig,dummy.getpacketsignature(),psigsize);
		fixedheadersize=dummy.getheadersize();		
	}

	template <class PacketClass> packetengine<PacketClass>::~packetengine () {
		accumulator.freebuf();
	}

	template <class PacketClass> void packetengine<PacketClass>::setuserobject(void *usrobj)
	{
		userobject=usrobj;
	}
	template <class PacketClass> void* packetengine<PacketClass>::getuserobject()
	{
		return userobject;
	}
	template <class PacketClass> void packetengine<PacketClass>::setonpacketreceived(void (*_onpacketreceived) (packetengine *, PacketClass &))
	{
		onpacketreceived=_onpacketreceived;
	}
	template <class PacketClass> void packetengine<PacketClass>::setondisconnected(void (*_ondisconnected) (packetengine *))
	{
		ondisconnected=_ondisconnected;
	}
	template <class PacketClass> void packetengine<PacketClass>::setcallbacks(void (*_onpacketreceived) (PacketClass &), void (*_ondisconnected) ())
	{
		setonpacketreceived(_onpacketreceived);
		setondisconnected(_ondisconnected);
	}
	template <class PacketClass> void packetengine<PacketClass>::useacceptedsocket(SOCKET connectedsock,char *RIP, int Port)
	{
		ts->useacceptedsocket(connectedsock,RIP,Port);
	}

	template <class PacketClass> bool packetengine<PacketClass>::connectto(char *ServerHost, int Port) {
		asocket::errors rv=ts->setremoteendpoint(ServerHost,Port);
		if (rv!=asocket::success) return FALSE;

		rv=ts->connect();

		if (rv!=asocket::success) return false;

		return true;
	}
	template <class PacketClass> void packetengine<PacketClass>::close()
	{
		ts->close();
	}
    template <class PacketClass> void packetengine<PacketClass>::forceclose()
    {
        ts->forceshutdown();
    }
	template <class PacketClass> void packetengine<PacketClass>::sendpacket(PacketClass *pbc) {
			SyncMutex.Wait();
			ts->send((char *)pbc->getpacket(),pbc->getpacketsize());
			SyncMutex.Release();
	}

ns1end

#endif

// This class takes a template parameter at design time of the Packet class
// This Packet class should be inhereted from PacketBase for the reason of
// implementing the 2 virtual functions 
//	virtual	long returnpacketsizefromheaderdata(void *packetheader)
//	virtual long getheadersize()
// This engine has an underlying socket class object TCPSock
// and it can connect, it handles the received data and with the help of the packetreceiver class
// it will extract pure packets of the type in the template
// and calls back a user defined callback with a filled pure packet of the type in the template
// it calls back also for a Close on the socket
// if callbacks are not specified to the class it will not throw an error but the user code won't be
// alerted otherwise of data arrivals or close events
// these callbacks can be specified as follows:
// [Object of this class].PacketReceived=&[userdefied packet received event];
// [Object of this class].EngineClosed=&[userdefied socket closed event];

// implementation of the callback functions as follows:
// PacketArrival:
// void [packet received function name]([Packet class as specified in the template] &rcp)  // e.g. LightPacket, or RCPacket
// Close event:
// void [engine closed function name]()

// Example: for packet received
// void PacketReceivedHere(RCPacket &rcp) {
	//smartbuf v;
	//long pc;
	//char bb[10];

	//::MessageBoxA(NULL,"OK HERE I AM","OK MAN",MB_OK);
	//rcp.GetLongTextDataItemCopy(0,pc,v);
	//ltoa(pc,bb,16);
	//::MessageBoxA(NULL,(char *)v.buffer,(char *)bb,MB_OK);
// }
// For Close
// void EngClosed()
// {
// ::MessageBoxA(NULL,"Engine closed","MyProgram",MB_OK);
// }

// This class as well as the TCPSock, packetreceiver, PacketBase, LightPacket
// are all written by Anmar Hassan, July 2008...