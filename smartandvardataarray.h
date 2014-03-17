//
//  smartarray.h
//  AHCLibCP
//
//  Created by Admin on 2/25/14.
//  Copyright (c) 2014 Anmar Al-Tameemi. All rights reserved.
//

#ifndef __AHCLibCP__smartarray__
#define __AHCLibCP__smartarray__
#include "lib.h"
#include "general.h"

ns1(general)
// SmartArray class declaration Start *********************************
template <class T> class SmartArray : public DataArray {
    
public:
    //DataArray DA();
    void Addfrombuffer(void *data,long itemscount);
    void Additem(T &t);
    T* operator [] (long index);
    void RemoveItems(long index, long nitems);
    void RemoveItem(long index);
    void RemoveLeft(long nitems);
    void RemoveRight(long nitems);
    SmartArray();
    void DeleteAllItems();
    ~SmartArray();
};
// SmartArray class declaration End ***********************************

// SmartArray class implementation Start *********************************
template <class T> void SmartArray<T>::Addfrombuffer(void *data,long itemscount){
    DataArray::Addfrombuffer(data,itemscount);
}

template <class T> void SmartArray<T>::Additem(T &t){
    DataArray::Addfrombuffer((void*)&t,1);
}
template <class T>  T*  SmartArray<T>::operator [] (long index) {
    return (T *)(DataArray::operator [](index));
}
template <class T> void SmartArray<T>::RemoveItems(long index, long nitems)
{	DataArray::RemoveItems(index,nitems);
}
template <class T> void SmartArray<T>::RemoveItem(long index)
{
    DataArray::RemoveItem(index);
}
template <class T> void SmartArray<T>::RemoveLeft(long nitems)
{
    DataArray::RemoveItems(0,nitems);
}
template <class T> void SmartArray<T>::RemoveRight(long nitems)
{	DataArray::RemoveRight(nitems);
}

template <class T> SmartArray<T>::SmartArray(){
    DataArray::SetUnitSize(sizeof(T));
}

template <class T> void SmartArray<T>::DeleteAllItems() {
    DataArray::DeleteAllItems();
}
template <class T> SmartArray<T>::~SmartArray() {
    DeleteAllItems();
    //if (buffinit==TRUE) DeleteAllItems();
}


// SmartArray class implementation End ***********************************

// VarDataArray class declaration Start *********************************
class VarDataArray
{
public:
    struct VarDataItem
    {
        DWORD offset;
        DWORD size;
        DWORD Type; // Optional
        DWORD userdata1;	// Optional
    };
    SmartArray<VarDataItem> items;
    
    smartbuf databuf;
    void PushAtEnd(void *Data, DWORD size, DWORD itemtype=0, DWORD _userdata1=0);
    void PushIn(void *Data, DWORD size, DWORD index, DWORD itemtype=0);
    void PushAtBegin(void *Data, DWORD size, DWORD itemtype=0);
    void SetDataAtItem_nosizechange(void *Data, DWORD datasize, long index);
    void RemoveItem(DWORD index);
    void RemoveItems(DWORD startIndex, DWORD count);
    void DeleteAllItems();
    VarDataItem * operator [] (long index);
    bool PopFromEnd(smartbuf &buf);
    bool PopFromBegin(smartbuf &buf);
    DWORD GetItemType(DWORD index);
    DWORD GetItemSize(DWORD index);
    DWORD Count();
    DWORD TotalSize();
    void *GetItemData(DWORD index);
    VarDataItem *GetItemInfo(DWORD index);
    void GetItem(DWORD index, smartbuf &item);
    
    VarDataArray();
    ~VarDataArray();
    
};

// VarDataArray class declaration End ***********************************
ns1end
#endif /* defined(__AHCLibCP__smartarray__) */
