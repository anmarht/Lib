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

#ifndef _AnmarLib_lightpacket_H_
#define _AnmarLib_lightpacket_H_

#include "lib.h"
#include "general.h"
#include "PacketBase.h"

#ifdef __APPLE__
#ifdef __LP64__
#define oldlong int
#else
#define oldlong long
#endif
#endif

ns1(net)



class LightPacket: public PacketBase { // Version 1.0 by Anmar Hassan, July 2008
    libroot::general::smartbuf buf;
  

    struct header {
        char packetsignature[6];
        oldlong datalength;
        oldlong CommandID;
    } LPHeader, *PLPHeader;

    
    char packetsig[6];
    void UpdateHeader();
    void initializepacket();
    void buildheader(oldlong CmdID);
    void setsizes();
public:
    oldlong packetsize;
    oldlong datasize;
    LightPacket ();
    LightPacket (oldlong CmdID);
    LightPacket (void *LightPacketRawData,oldlong size);
    ~LightPacket ();
    void *getpacket();
    oldlong getheadersize();
	oldlong getpacketsize();
    oldlong returnpacketsizefromheaderdata(void *packetheader);
    void SetPacketFromRawData(void *LightPacketRawData,oldlong size);
    bool GetDataItem(oldlong index, char *(&name), oldlong &namesize, char *(&value), oldlong &valuesize);
    bool GetDataItem(oldlong index,libroot::general::smartbuf &name,libroot::general::smartbuf &value);
    bool GetTextDataItem(oldlong index,libroot::general::smartbuf &name,libroot::general::smartbuf &value);
    oldlong GetDataItemCount();
    void SetCommand(oldlong CommandID);
    oldlong GetCommand();
    void AddDataItem(char *name,oldlong namesize,char *value, oldlong valuesize);
    void AddTextDataItem(char *name, char *value);
    void AddLongBinaryDataItem(oldlong ParameterCode, char *Data, oldlong Datasize);
    void AddLongLongDataItem(oldlong ParameterCode, oldlong Value);
    void AddLongTextDataItem(oldlong ParameterCode, char *DataText);
    bool GetLongTextDataItem(oldlong index,oldlong &ParameterCode,libroot::general::smartbuf &value);
    bool GetLongDataItem(oldlong index,oldlong &ParameterCode,libroot::general::smartbuf &value);
    bool GetLongItemValue(oldlong ParameterCode, libroot::general::smartbuf &value);
    bool GetLongLongItem(oldlong ParameterCode, oldlong &Value);
    bool GetLongItemValueText(oldlong ParameterCode, libroot::general::smartbuf &value);
    
    void cleardata();
    char *getpacketsignature();
    oldlong getpacketsignaturesize();
};

ns1end

#endif