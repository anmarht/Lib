#pragma once

#ifndef _AnmarLib_packetbase_H_
#define _AnmarLib_packetbase_H_

#include "lib.h"
#include "general.h"

#ifdef __APPLE__
#ifdef __LP64__
#define oldlong int
#else
#define oldlong long
#endif
#endif

ns1(net)

        class PacketBase {
        public:
            PacketBase();
            ~PacketBase();
            
            virtual	oldlong returnpacketsizefromheaderdata(void *packetheader); // This function should be overloaded to return the packet size stored in the header, used by packetreceived to extract packets during a receive operation
            virtual oldlong getheadersize();
            virtual char *getpacketsignature();
            virtual oldlong getpacketsignaturesize();
            
            // Added on 25 Feb 2014
            virtual void SetPacketFromRawData(void *LightPacketRawData, oldlong size);
            virtual void *getpacket();
            virtual oldlong getpacketsize();
        };
        
ns1end

#endif