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

#include "packetengine2.h"

ns1(net)



void packetengine2::onreceivestatic(asocket *sock, libroot::general::smartbuf &buf, char *remoteip, int remoteport)
{
    packetengine2 *peobj=(packetengine2 *)sock->getuserobject();
    peobj->onreceive(sock,buf,remoteip,remoteport);
}

void packetengine2::onreceive(asocket *sock, libroot::general::smartbuf &buf, char *remoteip, int remoteport)
{
    // Receive in a buffer,
    // keep accumulating until u reach the size of the fixed header
    // when size of the fixed header is reached, keep accumulating until u reach the size of the full packet, then dispatch.
    
    accumulator.push((char *)buf.buffer,buf.len);
again:
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
            thispacketlength=dummy->returnpacketsizefromheaderdata((void *)((char *)accumulator.buffer+pos));
            state=2;
        }
    }
    if (state==2)
    {
        if ((accumulator.len-pos)>=thispacketlength)
        {
            // We've got a pure packet, take it out, throw the garbage down to the begining, and keep the data after.
            
           // PacketBase newpacket;
            dummy->SetPacketFromRawData((void *)((char *)accumulator.buffer+pos),thispacketlength);
			//	libroot::general::smartbuf purepacket;
			//	purepacket.push((char *)accumulator.buffer+pos,thispacketlength);
            accumulator.cropleft(pos+thispacketlength);
            
            state=0;
            
            if (onpacketreceived!=NULL) onpacketreceived(this, *dummy);
            goto again;
        }
        
    }
    
}

void packetengine2::ondisconnectstatic(asocket *sock)
{
    packetengine2 *peobj=(packetengine2 *)sock->getuserobject();
    peobj->ondisconnect(sock);
}

void packetengine2::ondisconnect(asocket *sock)
{
    if (ondisconnected!=NULL) ondisconnected(this);
}

packetengine2::packetengine2 (PacketBase *yourpacketclass) {
    ts=new asocket(asocket::_TCP);
    ts->setOnReceive(&onreceivestatic);
    ts->setOnDisconnect(&ondisconnectstatic);
    ts->setuserobject(this);
    state=0;
    pos=0;
    dummy=yourpacketclass;
    psigsize=dummy->getpacketsignaturesize();
    strncpy(psig,dummy->getpacketsignature(),psigsize);
    fixedheadersize=dummy->getheadersize();
}

packetengine2::~packetengine2 () {
    accumulator.freebuf();
}

void packetengine2::setuserobject(void *usrobj)
{
    userobject=usrobj;
}
void* packetengine2::getuserobject()
{
    return userobject;
}
void packetengine2::setonpacketreceived(void (*_onpacketreceived) (packetengine2 *, PacketBase &))
{
    onpacketreceived=_onpacketreceived;
}
void packetengine2::setondisconnected(void (*_ondisconnected) (packetengine2 *))
{
    ondisconnected=_ondisconnected;
}
void packetengine2::setcallbacks(void (*_onpacketreceived) (packetengine2 *, PacketBase &), void (*_ondisconnected) (packetengine2 *))
{
    setonpacketreceived(_onpacketreceived);
    setondisconnected(_ondisconnected);
}
void packetengine2::useacceptedsocket(SOCKET connectedsock,char *RIP, int Port)
{
    ts->useacceptedsocket(connectedsock,RIP,Port);
}

bool packetengine2::connectto(char *ServerHost, int Port) {
    asocket::errors rv=ts->setremoteendpoint(ServerHost,Port);
    if (rv!=asocket::success) return FALSE;
    
    rv=ts->connect();
    
    if (rv!=asocket::success) return false;
    
    return true;
}
void packetengine2::close()
{
    ts->close();
}
void packetengine2::forceclose()
{
    ts->forceshutdown();
}
void packetengine2::sendpacket(PacketBase *pbc) {
    SyncMutex.Wait();
    ts->send((char *)pbc->getpacket(),pbc->getpacketsize());
    SyncMutex.Release();
}

ns1end


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