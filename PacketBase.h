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