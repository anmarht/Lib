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