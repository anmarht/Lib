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
#include "PacketBase.h"

ns1(net)
	// PacketBase Class Implementation Start *****************************
	PacketBase::PacketBase()
	{
	}
	PacketBase::~PacketBase()
	{
	}
	oldlong PacketBase::returnpacketsizefromheaderdata(void *packetheader) // This function should be overloaded to return the packet size stored in the header, used by packetreceived to extract packets during a receive operation
	{
		return -1;
	}
	oldlong PacketBase::getheadersize()
	{
		return -1;
	}
	char *PacketBase::getpacketsignature()
	{
		return NULL;
	}
	oldlong PacketBase::getpacketsignaturesize()
	{
		return -1;
	}

    // Added on 25 Feb 2014
    void PacketBase::SetPacketFromRawData(void *LightPacketRawData, oldlong size)
    {
        return;
    }
    void *PacketBase::getpacket()
    {
        return NULL;
    }
    oldlong PacketBase::getpacketsize()
    {
        return -1;
    }
	// PacketBase Class Implementation End *******************************

ns1end