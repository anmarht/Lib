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
#ifndef _AnmarLib_general_H_
#define _AnmarLib_general_H_

#include "lib.h"

ns1(general)

		void mid(char *str,long start, long len,char *ResultBuffer);
		bool SplitStr(char *str, char *SplitStr, long Index, char *ResultBuffer);
		long SubStrCount(char *str, char *SplitStr);
		long instr(char *MainStr, char *SubStr,long Occurance);
		long instrbinary(char *MainStr, char *SubStr,long MainSize, long SubSize,long Occurance);
		void clearmem(void *mem, DWORD size);
		bool IsNumeral(char *str);
		long vallong(char *str);
		long vallong(TCHAR *str);
		long valfromhexstr(char *str);
		long getlenW(LPWSTR widestr);
		void addDWtoDWDW(DWORD &dwhi,DWORD &dwlo, DWORD newlo);
		void subDWDWfromDWDW(DWORD &dwhi,DWORD &dwlo,DWORD newhi, DWORD newlo);
		void MBtoWC(char *dst, char *src,int srcsize, int dstmaxcount, bool forceUTF16);
		void MBtoWCUTF32(char *dst, char *src,int srcsize, int dstmaxcount);
		long getlenUTF16(char * widestr);
		long getlenUTF32(char * widestr);
		long getlenw(wchar_t *src);
		void mem_copy(void *dst, void *src, long size);
		#if defined(_usewindows_)	
			int ToUTF16(char *src, TCHAR *dst,int dstbufferSize);
			int ToUTF16withCP(UINT CP,TCHAR *dst, char *src,int dstbufferSize);
			int FromUTF16(char *dst, TCHAR *src,int dstbufferSize);
			int FromUTF16withCP(UINT CP,char *dst, TCHAR *src,int dstbufferSize);
		#endif


		// smartbuf class declaration Start ***********************************
		// a Complete push pull pop buffer class, don't worry anymore about buffers
		class smartbuf {
            bool buffinit;
			
		public:
            void *buffer;
            long len;
            static long debugObjsNum;
			void push(char *data,long size);
			void setbuffer(void *data,long size);
			bool updatebuffer(void *Data, long size, long position);
			void cropleft(long size);
			void cropright(long size);
			void pushin(void *data,long start, long size);
			void deleteat(long index, long size);
			long instr(void *substr,long size,long start, long iter);
			long instr(void *substr,long size,long start);
			void resize(long size);
			void freebuf();
			smartbuf();
			smartbuf(long size);
			smartbuf(void *data,long size);
			void dispose();
			~smartbuf();
		};
		// smartbuf class declaration End *************************************

		// DataArray class declaration Start *********************************
		class DataArray {   // Upon instantiation the programmer should specify the number of bytes. i.e. DataArray DA(10);  // for ten byte a record data array

            bool buffinit;


		protected:

		public:
            long unit_size; // Don't change this member, only read if you want
            void *buffer;
            long count;
            long size;
			void Addfrombuffer(void *data,long itemscount);
			void AllocateBufferOnly(long itemscount);
			void Additem(void *t);
			void Insertitemsat(void *t,long itemscount,long index);
			void * operator [] (long index);
			void RemoveItems(long index, long nitems);
			void RemoveItem(long index);
			void RemoveLeft(long nitems);
			void RemoveRight(long nitems);
			void SetItem(void *t, long index);
			void SetUnitSize(long unitsize);
			DataArray(long unitsize);
			DataArray();   // Becareful when using this empty constructor, you HAVE to set the Unit Size after this
			void DeleteAllItems();
			~DataArray();
		};
		// DataArray class declaration End ***********************************

		

		

        class NVPair
        {
            smartbuf _name,_value;
        public:
            NVPair();
            ~NVPair();
            NVPair(char *txtname, char *txtvalue);
            NVPair(char *txtname, char *value, long size);
            void Settxt(char *txtname, char *txtvalue);
            void Set(char *txtname, char *value, long size);
        };


ns1end

#endif
