//
//  smartarray.cpp
//  AHCLibCP
//
//  Created by Admin on 2/25/14.
//  Copyright (c) 2014 Anmar Al-Tameemi. All rights reserved.
//

#include "smartandvardataarray.h"

ns1(general)

// VarDataArray class implementation Start *********************************
void VarDataArray::PushAtEnd(void *Data, DWORD size, DWORD itemtype, DWORD _userdata1)
{	if (size<=0) return;
    VarDataItem vdi;
    vdi.offset=databuf.len;
    vdi.size=size;
    vdi.Type=itemtype;
    vdi.userdata1=_userdata1;
    items.Additem(vdi);
    databuf.push((char *)Data,size);
}
void VarDataArray::PushIn(void *Data, DWORD size, DWORD index, DWORD itemtype)
{	VarDataItem newitem;
    long i;
    if ((size<=0)||(index<0)) return;
    if (index>=items.count)
    {
        PushAtEnd(Data,size,itemtype);
        return;
    }
    newitem.offset=items[index]->offset;
    newitem.size=size;
    newitem.Type=itemtype;
    items.Insertitemsat(&newitem,1,index);
    for (i=index+1;i<items.count;i++)
    {
        items[i]->offset+=size;
    }
    databuf.pushin(Data,newitem.offset,newitem.size);
}
void VarDataArray::SetDataAtItem_nosizechange(void *Data, DWORD datasize, long index)
{
    long alloweddatasize;
    if ((index<0)||(index>=items.count)) return;
    if (items[index]->size<datasize)
        alloweddatasize=items[index]->size;
    else alloweddatasize=datasize;
    databuf.updatebuffer(Data,alloweddatasize,items[index]->offset);
    items[index]->userdata1=alloweddatasize;
}
void VarDataArray::PushAtBegin(void *Data, DWORD size, DWORD itemtype)
{
    PushIn(Data,size,0,itemtype);
}
void VarDataArray::RemoveItem(DWORD index)
{	long i;
    DWORD oldsiz;
    if ((index<0)||(index>=items.count)) return;
    oldsiz=items[index]->size;
    databuf.deleteat(items[index]->offset,items[index]->size);
    items.RemoveItem(index);
    for (i=index;i<items.count;i++)
    {
        items[i]->offset-=oldsiz;
    }
}
void VarDataArray::RemoveItems(DWORD startIndex, DWORD count)
{	DWORD cnt,i;
    if ((startIndex<0)||(startIndex>=items.count)) return;
    if (count<0) return;
    if ((count+startIndex)>items.count) cnt=items.count-startIndex; else cnt=count;
    for (i=0;i<cnt;i++)
    {
        RemoveItem(startIndex);
    }
}
void VarDataArray::DeleteAllItems()
{
    items.DeleteAllItems();
    if (databuf.len>0) databuf.freebuf();
}
DWORD VarDataArray::GetItemSize(DWORD index)
{	long siz;
    if ((index<0)||(index>=items.count)) return 0;
    siz=items[index]->size;
    return siz;
}
DWORD VarDataArray::GetItemType(DWORD index)
{	long type;
    if ((index<0)||(index>=items.count)) return 0;
    type=items[index]->Type;
    return type;
}
void *VarDataArray::GetItemData(DWORD index)
{
    if ((index<0)||(index>=items.count)) return 0;
    return (char *)((char *)databuf.buffer+items[index]->offset);
}
VarDataArray::VarDataItem *VarDataArray::GetItemInfo(DWORD index)
{
    if ((index<0)||(index>=items.count)) return 0;
    return items[index];
}
VarDataArray::VarDataItem * VarDataArray::operator [] (long index)
{
    return GetItemInfo(index);
}
void VarDataArray::GetItem(DWORD index, smartbuf &item)
{
    if ((index<0)||(index>=items.count))
    {
        item.freebuf();
        return;
    }
    item.setbuffer((char *)databuf.buffer+items[index]->offset,items[index]->size);
}
bool VarDataArray::PopFromEnd(smartbuf &buf)
{
    if (items.count==0) return false;
    GetItem(items.count-1,buf);
    RemoveItem(items.count-1);
    return true;
}
DWORD VarDataArray::Count()
{
    return items.count;
}
DWORD VarDataArray::TotalSize()
{
    return databuf.len;
}
bool VarDataArray::PopFromBegin(smartbuf &buf)
{	DWORD i,oldsize;
    if (items.count==0) return false;
    GetItem(0,buf);
    //oldsize=items[0]->size;
    RemoveItem(0);
    //	for (i=0;i<items.count;i++) items[i]->offset-=oldsize;
    return true;
}
VarDataArray::VarDataArray()
{
}
VarDataArray::~VarDataArray()
{
}
ns1end
// VarDataArray class implementation End ***********************************