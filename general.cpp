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
#include "general.h"
#include "stdlib.h"
#include "string.h"

ns1(general)


long getlenUTF16(char * widestr)
{	long i;
	short *p;
	if (widestr==NULL) return 0;
	p=(short *)widestr;i=0;
	while (*p!=0) {p++;i++;}
	 return i;			
}

long getlenUTF32(char * widestr)
{	long i;
	long *p;
	if (widestr==NULL) return 0;
	p=(long *)widestr;i=0;
	while ((*p&0xffffffff)!=0) {i++;p=(long *)((char *)widestr+i*4);}
    return i;
}
#if defined(_usewindows_)	

	int ToUTF16(char *src, TCHAR *dst,int dstbufferSize)
	{
		return MultiByteToWideChar(CP_UTF8,0,src,-1,dst,dstbufferSize);
	}
	int ToUTF16withCP(UINT CP,TCHAR *dst, char *src,int dstbufferSize)
	{
		return MultiByteToWideChar(CP,0,src,-1,dst,dstbufferSize);
	}
	int FromUTF16(char *dst, TCHAR *src,int dstbufferSize)
	{
		return WideCharToMultiByte(CP_UTF8,0,src,-1,dst,dstbufferSize,NULL,NULL);
	}
	int FromUTF16withCP(UINT CP,char *dst, TCHAR *src,int dstbufferSize)
	{
		return WideCharToMultiByte(CP,0,src,-1,dst,dstbufferSize,NULL,NULL);	
	}
	void MBtoWC(char *dst, char *src,int srcsize, int dstmaxcount, bool forceUTF16)
	{
		mbstowcs ((wchar_t *)dst, src, dstmaxcount);
		return;
	}
	void MBtoWCUTF32(char *dst, char *src,int srcsize, int dstmaxcount)
	{
		int length;
		char *mydst=dst;
		char tmpdst[4]={0,0,0,0};
		char *mysrc=src;
		mbtowc (NULL, NULL, 0);
		int left=srcsize;
		while (left>0) {
			length = mbtowc((wchar_t *)tmpdst,mysrc,2);
			if (length<1) break;
			// copy 2 chars from tmpdsk to mydst
			mem_copy(mydst,tmpdst,4);
			mysrc+=length; mydst+=4; left-=length;
		}
		return;
	}

	long getlenw(wchar_t *src)
	{
		return getlenUTF16((char *)src);
	}
#endif
#if defined(_useposix_) && !defined(_useandroid_)

	void MBtoWC(char *dst, char *src,int srcsize, int dstmaxcount, bool forceUTF16)
	{
		if (!forceUTF16)
		{
			mbstowcs ((wchar_t *)dst, src, dstmaxcount);
			return;
		}
		else
		{
			int length;
			char *mydst=dst;
			wchar_t tmpdst[4];
			char *mysrc=src;
			mbtowc (NULL, NULL, 0);
			int left=srcsize;
			while (left>0) {
				length = mbtowc(tmpdst,mysrc,4);
				if (length<1) break;
				// copy 2 chars from tmpdsk to mydst
				mem_copy(mydst,tmpdst,2);
				mysrc+=length; mydst+=2; left-=length;
			}
		}
	}
	void MBtoWCUTF32(char *dst, char *src,int srcsize, int dstmaxcount)
	{
		mbstowcs ((wchar_t *)dst, src, dstmaxcount);
	}

#endif
#if defined(_useposix_)
	long getlenw(wchar_t *src)
	{
		return getlenUTF32((char *)src);
	}
#endif





		long vallong(char *str)
		{
			return atol(str);
		}

		void mid(char *str,long start, long len,char *ResultBuffer) {
		char *p,*pp;
		long i;

		if ((str==NULL)||(ResultBuffer==NULL)||(len<=0)||(start<0)) {*ResultBuffer=0;return;}
		if (start>=strlen(str)) {*ResultBuffer=0;return;}
			for (i=0;i<len;i++) {
		*(ResultBuffer+i)=*(str+start+i);

			}
			*(ResultBuffer+i)=0;
		}

		bool SplitStr(char *str, char *SplitStr, long Index, char *ResultBuffer) {
			long i,ci=0,qi=0;

			for (i=0;i<=(strlen(str));i++)
			{
				if ((strncmp(str+i,SplitStr,strlen(SplitStr))==0)||(i==(strlen(str)))) {
					if (Index==ci) {
						mid(str,qi,i-qi,ResultBuffer);
						return TRUE;
					}
					ci++;qi=i+strlen(SplitStr);
				}
			}
			return FALSE;
		}
		long SubStrCount(char *str, char *SplitStr) {
		long i,ci=0;
		if ((str==NULL)||(SplitStr==NULL)) return 0;
		if ((strlen(str)==0)||(strlen(SplitStr)==0)) return 0;
		for (i=0;i<=(strlen(str)-strlen(SplitStr));i++)
		{
			if (strncmp(str+i,SplitStr,strlen(SplitStr))==0)
			ci++;

		}
		return ci;
		}

		long instr(char *MainStr, char *SubStr,long Occurance) {
		long i,ci=0;
		if ((MainStr==NULL)||(SubStr==NULL)) return -1;
		if ((strlen(MainStr)==0)||(strlen(SubStr)==0)) return -1;
		for (i=0;i<=(strlen(MainStr)-strlen(SubStr));i++)
		{
			if (strncmp(MainStr+i,SubStr,strlen(SubStr))==0)
			{
				if (ci==Occurance) return i;
				ci++;
			}


		}
		return -1;
		}
		long instrbinary(char *MainStr, char *SubStr,long MainSize, long SubSize,long Occurance) {
		long i,ci=0;
		if ((MainStr==NULL)||(SubStr==NULL)) return -1;
		if ((MainSize==0)||(SubSize==0)) return -1;
		for (i=0;i<=(MainSize-SubSize);i++)
		{
			if (strncmp(MainStr+i,SubStr,SubSize)==0)
			{
				if (ci==Occurance) return i;
				ci++;
			}

		}

		return -1;
		}

		void mem_copy(void *dst, void *src, long size)
		{
			memmove(dst,src,size);
		}

		bool IsNumeral(char *str)
		{
		long i;
		for (i=0;i<strlen(str);i++)
		if (((*(str+i))<'0')||(((*(str+i))>'9'))) return FALSE;
		return TRUE;
		}

		// smartbuf class implementation ****************************************


		void smartbuf::push(char *data,long size) {
			if (buffinit==false) {
				buffer=malloc(size);
				if (buffer==0)
				{ //print("\n Error memmalloc push size=%d",size);
					return;
				}
				len=size;
				buffinit=true;
				mem_copy(buffer,data,size);
			}
			else
			{ buffer=realloc(buffer,len+size);
				if (buffer==NULL)
				{
					return;
					//	print("\n Error memrealloc push size=%d",len+size);

				}
				mem_copy(((char *)buffer+len),data,size);
				len+=size;
			}
		}
		void smartbuf::setbuffer(void *data,long size) {
			if (buffinit==false) {
				buffer=malloc(size);
				if (buffer==0)
				{
					return;
					//print("\n Error memmalloc setbuffer size=%d",size);

				}
				len=size;
				buffinit=true;
				mem_copy(buffer,data,size);
			}
			else
			{	 buffer=realloc(buffer,size);
				//		if (buffer==NULL) print("\nError memrealloc setbuffer size=%d",size);
				mem_copy(buffer,data,size);
				len=size;
			}
		}
		bool smartbuf::updatebuffer(void *Data, long size, long position) {
			if ((position+size)>len) return false;
			mem_copy((char *)buffer+position,Data,size);
			return true;
		}
		void smartbuf::pushin(void *data,long start, long size)
		{	long ln;
			if (start<0) return;
			if (size<=0) return;
			if (start>=len) {push((char *)data,size);return;}
			ln=len;
			resize(len+size);			
			mem_copy((char *)buffer+start+size,(char *)buffer+start,ln-start);
			updatebuffer(data,size,start);
		}
		long smartbuf::instr(void *substr,long size,long start, long iter)
		{
			long i,curriter;  // anmar hassan
			// 012345678901
			curriter=0;
			for (i=start;i<=len-size;i++)
			{
				if (memcmp((char *)buffer+i,substr,size)==0) {
					curriter++;
					if (curriter==iter) return i;
				}
			}
			return -1;
		}
		long smartbuf::instr(void *substr,long size,long start)
		{
			return instr(substr,size,start,1);
		}
		void smartbuf::cropleft(long size)
		{
			if (buffinit==false) return;
			if (len==0) return;
			mem_copy(buffer,(char *)buffer+size,len-size);
			buffer=realloc(buffer,len-size);
			//	if (buffer==NULL) print("\n Error memrealloc cropleft size=%d",len-size);
			len-=size;
		}
		void smartbuf::cropright(long size)
		{
			if (buffinit==false) return;
			if (len==0) return;
			buffer=realloc(buffer,len-size);
			//	if (buffer==NULL) print("\n Error memrealloc cropright size=%d",len-size);
			len-=size;
		}
		void smartbuf::deleteat(long index, long size)
		{	long src,cpysiz,ssize;
			if (index<0) return;
			if (index>len) return;
			if (size<=0) return;
			ssize=size;
			src=index+ssize;
			if (src>len) {src=len;ssize=len-index;}
			cpysiz=len-src;
			if (cpysiz>0)
			{
				mem_copy((char *)buffer+index,(char *)buffer+src,cpysiz);}
			len-=ssize;
			buffer=realloc(buffer,len);
			//if (buffer==NULL) print("\n Error memrealloc deleteat size=%d",len);
		}
		void smartbuf::resize(long size)
		{	if (size<0) return;
			if (buffinit==false) {
				buffer=malloc(size);
				if (buffer==0)
				{//print("\n Error memmalloc resize size=%d",size);
					return;
				}
				len=size;
				buffinit=true;
			}
			else
			{	buffer=realloc(buffer,size);
				//			if (buffer==NULL) print("\n Error memrealloc resize size=%d",size);
				len=size;
			}
		}
		void smartbuf::freebuf()
		{if (buffinit==true) {
			//	print("\nmemfree buffer=%x",buffer);
			free(buffer);
			len=0;
			buffinit=false;}
		}

		smartbuf::smartbuf(){
			buffinit=false;
			len=0;
			debugObjsNum++;
		}
		smartbuf::smartbuf(long size){
			buffer=malloc(size);
			if (buffer==0)
			{//print("\n Error memmalloc constructor size=%d",size);
				return;
			}
			len=size;
			buffinit=true;
			debugObjsNum++;
		}
		smartbuf::smartbuf(void *data,long size){
			setbuffer(data,size);
			debugObjsNum++;
		}

		void smartbuf::dispose()
		{
			
			if (buffinit==true) {freebuf();debugObjsNum--; }
		}

		smartbuf::~smartbuf() {
			dispose();
		}

		long smartbuf::debugObjsNum=0;
		/// smartbuf class implementation end ************************************

		/// NameValuePair implementation start *********************
		NVPair::NVPair()
		{

		}
		NVPair::NVPair(char *txtname, char *txtvalue)
		{
			Settxt(txtname,txtvalue);
		}
		NVPair::NVPair(char *txtname, char *value, long size)
		{
			Set(txtname, value, size);
		}
		void NVPair::Settxt(char *txtname, char *txtvalue)
		{
			_name.setbuffer(txtname,strlen(txtname));_name.push((char *)"\0", 1);
			_value.setbuffer(txtvalue,strlen(txtvalue));_value.push((char *)"\0", 1);
		}
		void NVPair::Set(char *txtname, char *value, long size)
		{
			_name.setbuffer(txtname,strlen(txtname));_name.push((char *)"\0", 1);
			_value.setbuffer(value,size);
		}
		NVPair::~NVPair()
		{
			_name.freebuf();
			_value.freebuf();
		  //  NSLog(@"~NVPair() has run...");

		}
		/// NameValuePair implementation end *********************


		// DataArray class implementation Start *********************************
		void DataArray::Addfrombuffer(void *data,long itemscount){
			if (buffinit==false) {
				buffer=malloc(itemscount*unit_size);
				if (buffer==0)
				{
					//	MessageBox(NULL,L"ERROR memmalloc",L"ERROR SMARTBUF",MB_OK);
					return;
				}
				count=itemscount;size=itemscount*unit_size;
				buffinit=true;
				mem_copy(buffer,data,itemscount*unit_size);
			}
			else
			{buffer=realloc(buffer,(count+itemscount)*unit_size);
				mem_copy(((char *)buffer+count*unit_size),data,itemscount*unit_size);
				count+=itemscount;size=count*unit_size;
			}
		}
		void DataArray::AllocateBufferOnly(long itemscount)
		{
			if (buffinit==false) {
				buffer=malloc(itemscount*unit_size);
				if (buffer==0)
				{
					//MessageBox(NULL,L"ERROR memmalloc",L"ERROR SMARTBUF",MB_OK);
					return;

				}
				count=itemscount;size=itemscount*unit_size;
				buffinit=true;
			}
			else
			{buffer=realloc(buffer,(count+itemscount)*unit_size);
				count+=itemscount;size=count*unit_size;
			}
		}
		void DataArray::SetItem(void *data, long index)
		{
			if ((buffinit==true)&&(index<count))
			{
				mem_copy(((char *)buffer+index*unit_size),data,unit_size);
			}
		}
		void DataArray::Additem(void *object){
			Addfrombuffer(object,1);
		}
		void*  DataArray::operator [] (long index) {
			return (void *)((char *)buffer+index*unit_size);
		}
		void DataArray::RemoveItems(long index, long nitems)
		{	long src,cpysiz,nnitems;
			if (count==0) return;
			if (index<0) return;
			if (index>count) return;
			if (nitems<=0) return;
			nnitems=nitems;
			src=index+nitems;
			if (src>count) {src=count;nnitems=count-index;}
			cpysiz=size-src*unit_size;
			if (cpysiz>0)
			{
				mem_copy((char *)buffer+index*unit_size,(char *)buffer+src*unit_size,cpysiz);}
			count-=nnitems;size=count*unit_size;
			buffer=realloc(buffer,count*unit_size);
			if (buffer==0) buffinit=false;
		}
		void DataArray::Insertitemsat(void *t,long itemscount,long index)
		{
			if (index<0) return;
			if (itemscount<=0) return;
			if (index>=count)
			{
				Addfrombuffer(t,itemscount);
				return;
			}
			buffer=realloc(buffer,(count+itemscount)*unit_size);
			mem_copy((char *)buffer+(index+itemscount)*unit_size,(char *)buffer+index*unit_size,(count-index)*unit_size);
			mem_copy((char *)buffer+index*unit_size,(char *)t,itemscount*unit_size);
			count+=itemscount;size=count*unit_size;
		}
		void DataArray::RemoveItem(long index)
		{
			RemoveItems(index,1);
		}
		void DataArray::RemoveLeft(long nitems)
		{
			RemoveItems(0,nitems);
		}
		void DataArray::RemoveRight(long nitems)
		{	long nnitems;
			if (nitems>count) nnitems=count; else nnitems=nitems;
			RemoveItems(count-nnitems,nnitems);
		}

		DataArray::DataArray(){
			buffinit=false;
			unit_size=0;
			count=size=0;
		}

		DataArray::DataArray(long unitsize){
			buffinit=false;
			unit_size=unitsize;
			count=size=0;
		}
		void DataArray::SetUnitSize(long unitsize){
			unit_size=unitsize;
		}

		void DataArray::DeleteAllItems() {
			if (buffinit==true) {
				free(buffer);
				buffinit=false;
			}
			count=size=0;
		}
		DataArray::~DataArray() {
			if (buffinit==true) DeleteAllItems();
		}


		// DataArray class implementation End ***********************************



ns1end
