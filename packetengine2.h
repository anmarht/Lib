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
#ifndef _AnmarLib_PacketEngine2_H_
#define _AnmarLib_PacketEngine2_H_

#include "lib.h"
#include "asocket.h"
#include "netgeneral.h"
#include "general.h"
#include "sync.h"
#include "PacketBase.h"
#include <string.h>
ns1(net)

class packetengine2 {
    asocket *ts;
    //libroot::net::packetreceiver pr;
    
    PacketBase *dummy;
	
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
    
    void (*onpacketreceived) (packetengine2 *, PacketBase &);
    void (*ondisconnected)(packetengine2 *);
    
public:
    libroot::sync::Mutex SyncMutex;
    void *userobject;
    
    void setuserobject(void *usrobj);
    void *getuserobject();
    
    void setonpacketreceived(void (*_onpacketreceived) (packetengine2 *, PacketBase &));
    void setondisconnected(void (*_ondisconnected) (packetengine2 *));
    void setcallbacks(void (*_onpacketreceived) (packetengine2 *, PacketBase &), void (*_ondisconnected) (packetengine2 *));
    
    
    packetengine2 (PacketBase *yourpacketclass);
    ~packetengine2 ();
    void useacceptedsocket(SOCKET connectedsock,char *RIP, int Port);
    bool connectto(char *ServerHost, int Port);
    void close();
    void forceclose();
    void sendpacket(PacketBase *pbc);
    
};



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