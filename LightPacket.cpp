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

#include "LightPacket.h"
#include "string.h"

#ifdef __APPLE__
#ifdef __LP64__
#define oldlong int
#else
#define oldlong long
#endif
#endif

ns1(net)

// LightPacket Class Implementation Start *****************************

void LightPacket::UpdateHeader()
{   PLPHeader=(struct header *)buf.buffer;
    PLPHeader->datalength=buf.len-sizeof(header);
    setsizes();
    
}
void LightPacket::initializepacket()
{
    buf.setbuffer(&LPHeader,sizeof(header));
    setsizes();
}
void LightPacket::buildheader(oldlong CmdID)
{
    PLPHeader=(struct header *)buf.buffer;
    strncpy(PLPHeader->packetsignature,getpacketsignature(),getpacketsignaturesize());
    PLPHeader->CommandID=CmdID;
    setsizes();
}
void LightPacket::setsizes()
{
    packetsize=buf.len;
    datasize=buf.len-sizeof(header);
    PLPHeader=(struct header *)buf.buffer;
    PLPHeader->datalength=datasize;
}
oldlong LightPacket::getpacketsize()
{
	return packetsize;
}

LightPacket::LightPacket () {
    initializepacket();
    buildheader(0);
}
LightPacket::LightPacket (oldlong CmdID) {
    initializepacket();
    buildheader(CmdID);
}
LightPacket::LightPacket (void *LightPacketRawData,oldlong size) {
    SetPacketFromRawData(LightPacketRawData,size);
}
LightPacket::~LightPacket ()
{
    buf.freebuf();
}
void *LightPacket::getpacket() {
    return buf.buffer;
}
oldlong LightPacket::getheadersize() {
    return sizeof(header);
}
oldlong LightPacket::returnpacketsizefromheaderdata(void *packetheader) // This will return the packetsize by extracting data length from header + adding the constant header size
{
    PLPHeader=(struct header *)packetheader;
    return PLPHeader->datalength+sizeof(header);
}
void LightPacket::SetPacketFromRawData(void *LightPacketRawData,oldlong size)
{
    buf.setbuffer(LightPacketRawData,size);
    setsizes();
}

bool LightPacket::GetDataItem(oldlong index, char *(&name), oldlong &namesize, char *(&value), oldlong &valuesize)
{	oldlong i,p,*pp,s;bool found;
    i=0;found=FALSE;
    p=sizeof(header);
    if (index>=GetDataItemCount()) return FALSE;
    while ((p<packetsize)&&(i<=index)) {
        pp=(oldlong *)(p+(char *)buf.buffer);
        s=*pp; // name size
        p+=4;
        // now p points to name data
        if (i==index) {name=(char *)(p+(char *)buf.buffer);namesize=s;found=TRUE;}
        p+=s;
        pp=(oldlong *)(p+(char *)buf.buffer);
        s=*pp; // value size
        p+=4;
        // now p points to value data
        if (i==index) {value=(char *)(p+(char *)buf.buffer);valuesize=s;found=TRUE;}
        p+=s;i++;
    }
    if (found==TRUE) return TRUE; else {name=NULL;value=NULL;return FALSE;}
}


bool LightPacket::GetDataItem(oldlong index,libroot::general::smartbuf &name,libroot::general::smartbuf &value)
{	char *n,*v;
    oldlong ns,vs;
    if (GetDataItem(index,n,ns,v,vs))
    {name.setbuffer(n,ns);value.setbuffer(v,vs);return TRUE;} else return FALSE;
}
bool LightPacket::GetTextDataItem(oldlong index,libroot::general::smartbuf &name,libroot::general::smartbuf &value)
{
    if (GetDataItem(index,name,value))
    {name.push("\0",1);value.push("\0",1);return TRUE;} else return FALSE;
}
        
bool LightPacket::GetLongItemValue(oldlong ParameterCode, libroot::general::smartbuf &value)
{
    oldlong i;char *n,*v;oldlong ns,vs,pc;
    for (i=0;i<GetDataItemCount();i++)
    {
        GetDataItem(i,n,ns,v,vs);
        pc=*(oldlong *)n;
        if (pc==ParameterCode) {
            value.setbuffer(v,vs);
            return TRUE;
        }
        
    }
    return FALSE;
}
bool LightPacket::GetLongItemValueText(oldlong ParameterCode, libroot::general::smartbuf &value)
{
    if (GetLongItemValue(ParameterCode,value))
    {value.push("\0\0",2);return TRUE;}
    return FALSE;
}

oldlong LightPacket::GetDataItemCount()
{	oldlong i,p,*pp,s;
    i=0;
    p=sizeof(header);
    while (p<packetsize) {
        pp=(oldlong *)(p+(char *)buf.buffer);
        s=*pp; // name size
        p+=4;
        // now p points to name data
        p+=s;
        pp=(oldlong *)(p+(char *)buf.buffer);
        s=*pp; // value size
        p+=4;
        // now p points to value data
        p+=s;i++;
    }
    return i;
}


void LightPacket::SetCommand(oldlong CommandID)
{   PLPHeader=(struct header *)buf.buffer;
    PLPHeader->CommandID=CommandID;
}
oldlong LightPacket::GetCommand()
{   PLPHeader=(struct header *)buf.buffer;
    return PLPHeader->CommandID;
}


void LightPacket::AddDataItem(char *name,oldlong namesize,char *value, oldlong valuesize) {
    buf.push((char *)&namesize,sizeof(oldlong));
    buf.push(name,namesize);
    buf.push((char *)&valuesize,sizeof(oldlong));
    buf.push(value,valuesize);
    UpdateHeader();
}


void LightPacket::AddTextDataItem(char *name, char *value)
{
    AddDataItem(name,strlen(name),value,strlen(value));
}
void LightPacket::AddLongBinaryDataItem(oldlong ParameterCode, char *Data, oldlong Datasize)
{
    AddDataItem((char *)&ParameterCode,4,Data,Datasize);

}
void LightPacket::AddLongLongDataItem(oldlong ParameterCode, oldlong Value)
{
    AddDataItem((char *)&ParameterCode,4,(char *)&Value,4);
}
void LightPacket::AddLongTextDataItem(oldlong ParameterCode, char *DataText)
{
    AddDataItem((char *)&ParameterCode,4,DataText,strlen(DataText));
}
bool LightPacket::GetLongDataItem(oldlong index,oldlong &ParameterCode,libroot::general::smartbuf &value)
{   char *n,*v;
    oldlong *pc;
    oldlong ns,vs;
    if (GetDataItem(index,n,ns,v,vs))
    {pc=(oldlong *)n;
		ParameterCode=*pc;
		value.setbuffer(v,vs);
		return TRUE;} else return FALSE;
}
bool LightPacket::GetLongLongItem(oldlong ParameterCode, oldlong &Value)
{	oldlong i;char *n,*v;oldlong ns,vs,pc;
    for (i=0;i<GetDataItemCount();i++)
    {
        GetDataItem(i,n,ns,v,vs);
        pc=*(oldlong *)n;
        if (pc==ParameterCode) {
            Value=*(oldlong *)v;
            return TRUE;
        }
        
    }
    return FALSE;		
}
bool LightPacket::GetLongTextDataItem(oldlong index,oldlong &ParameterCode,libroot::general::smartbuf &value)
{   char *n,*v;
    oldlong *pc;
    oldlong ns,vs;
    if (GetDataItem(index,n,ns,v,vs))
    {pc=(oldlong *)n;
		ParameterCode=*pc;
		value.setbuffer(v,vs);
		value.push("\0",1);return TRUE;} else return FALSE;
}
void LightPacket::cleardata()
{	buf.resize(sizeof(header));
    setsizes();
}
char *LightPacket::getpacketsignature() {
    strcpy(packetsig,"LTPACK");
    return packetsig;
}
oldlong LightPacket::getpacketsignaturesize() {
    return 6;
}

// LightPacket Class Implementation END *******************************

ns1end