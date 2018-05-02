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

#ifndef _AnmarLib_str_H_
#define _AnmarLib_str_H_

#include "lib.h"
#include "general.h"
#include "textencoding.h"

ns1(general)

	class str
	{	
		int charsize;

		void SetDefaults();
		void pushnull();

	public:
		libroot::general::smartbuf buf, tmp;
		str();

		int GetCharSize();

		long Len();
		void set(char* cstr);
		void set(wchar_t* wstr);
		void set(unsigned long* utf32str);
		void set(unsigned short* utf16str);
		void set(str &strobj);
		void set(const str &strobj);
		void set(int number);
		void set(unsigned int number);
		void set(long number);
		void set(unsigned long number);
		void set(float number);
		void set(double number);

		void add(char* cstr);

		void add(wchar_t* wstr);
		void add(unsigned long* utf32str);
		void add(unsigned short* utf16str);
		void add(const str &strobj);
		void add(str &strobj);

		void add(int number);
		void add(unsigned int number);
		void add(long number);
		void add(unsigned long number);
		void addhex(int number, int padding=0);
		void addhex(unsigned int number, int padding=0);
		void addhex(long number, int padding=0);
		void addhex(unsigned long number, int padding=0);

		void add(float number);

		void add(double number);
		long getlength();
		long getsizebytes();

		void cropleft(long count);
		void cropright(long count);

		void cropmid(long start, long count);
	
		void setat(long index, wchar_t *src);
		void setat(long index, char *src);
		void setat(long index, unsigned long *src);
		void setat(long index, unsigned short *src);

		void setat(long index, str &src);
		char *getutf8();

		unsigned short *getutf16();
		unsigned long *getutf32();
		
		wchar_t *getwide();

		void insertat(long index, char *src);
		void insertat(long index, wchar_t *src);

		void insertat(long index, unsigned long *src);

		void insertat(long index, unsigned short *src);

		void insertat(long index, str &src);

		bool isequalto(str &strobj);

		bool isequalto(char *src);

		bool isequalto(wchar_t *src);

		bool isequalto(unsigned long *src);
	
		bool isequalto(unsigned short *src);

		void mid(long start, long count, str &targetstr);

		void left(long count, str &targetstr);

		void right(long count, str &targetstr);

		long instr(str &findstr, long startpos, int totaliterations=1);

		long instr(char *src, long startpos=0, int totaliterations=1);

		long instr(wchar_t *src, long startpos=0, int totaliterations=1);

		long instr(unsigned long *src, long startpos=0, int totaliterations=1);

		long instr(unsigned short *src, long startpos=0, int totaliterations=1);

		bool contains(str strobj);

		bool contains(char * strsrc);

		bool contains(wchar_t * strsrc);

		bool contains(unsigned long * strsrc);

		bool contains(unsigned short * strsrc);

		void replace(str &findstr, str &replacestr, long startpos);

		void replaceall(str &findstr, str &replacestr, long startpos);

		void replace(char *findsrc, char *replacesrc, long startpos=0);

		void replace(char *findsrc, wchar_t *replacesrc, long startpos=0);
		void replace(char *findsrc, unsigned long *replacesrc, long startpos=0);
		void replace(char *findsrc, unsigned short *replacesrc, long startpos=0);
		void replace(char *findsrc,  str &replacesrc, long startpos=0);

		void replace(wchar_t *findsrc, char *replacesrc, long startpos=0);
		void replace(wchar_t *findsrc, wchar_t *replacesrc, long startpos=0);
		void replace(wchar_t *findsrc, unsigned long *replacesrc, long startpos=0);
		void replace(wchar_t *findsrc, unsigned short *replacesrc, long startpos=0);
		void replace(wchar_t *findsrc,  str &replacesrc, long startpos=0);

		void replace(unsigned long *findsrc, char *replacesrc, long startpos=0);
		void replace(unsigned long *findsrc, wchar_t *replacesrc, long startpos=0);
		void replace(unsigned long *findsrc, unsigned long *replacesrc, long startpos=0);
		void replace(unsigned long *findsrc, unsigned short *replacesrc, long startpos=0);
		void replace(unsigned long *findsrc,  str &replacesrc, long startpos=0);

		void replace(unsigned short *findsrc, char *replacesrc, long startpos=0);
		void replace(unsigned short *findsrc, wchar_t *replacesrc, long startpos=0);
		void replace(unsigned short *findsrc, unsigned long *replacesrc, long startpos=0);
		void replace(unsigned short *findsrc, unsigned short *replacesrc, long startpos=0);
		void replace(unsigned short *findsrc,  str &replacesrc, long startpos=0);

		void replace(str &findsrc, char *replacesrc, long startpos=0);
		void replace(str &findsrc, wchar_t *replacesrc, long startpos=0);
		void replace(str &findsrc, unsigned long *replacesrc, long startpos=0);
		void replace(str &findsrc, unsigned short *replacesrc, long startpos=0);

		void replaceall(char *findsrc, char *replacesrc, long startpos=0);
		void replaceall(char *findsrc, wchar_t *replacesrc, long startpos=0);
		void replaceall(char *findsrc, unsigned long *replacesrc, long startpos=0);
		void replaceall(char *findsrc, unsigned short *replacesrc, long startpos=0);
		void replaceall(char *findsrc,  str &replacesrc, long startpos=0);

		void replaceall(wchar_t *findsrc, char *replacesrc, long startpos=0);
		void replaceall(wchar_t *findsrc, wchar_t *replacesrc, long startpos=0);
		void replaceall(wchar_t *findsrc, unsigned long *replacesrc, long startpos=0);
		void replaceall(wchar_t *findsrc, unsigned short *replacesrc, long startpos=0);
		void replaceall(wchar_t *findsrc,  str &replacesrc, long startpos=0);

		void replaceall(unsigned long *findsrc, char *replacesrc, long startpos=0);
		void replaceall(unsigned long *findsrc, wchar_t *replacesrc, long startpos=0);
		void replaceall(unsigned long *findsrc, unsigned long *replacesrc, long startpos=0);
		void replaceall(unsigned long *findsrc, unsigned short *replacesrc, long startpos=0);
		void replaceall(unsigned long *findsrc,  str &replacesrc, long startpos=0);

		void replaceall(unsigned short *findsrc, char *replacesrc, long startpos=0);
		void replaceall(unsigned short *findsrc, wchar_t *replacesrc, long startpos=0);
		void replaceall(unsigned short *findsrc, unsigned long *replacesrc, long startpos=0);
		void replaceall(unsigned short *findsrc, unsigned short *replacesrc, long startpos=0);
		void replaceall(unsigned short *findsrc,  str &replacesrc, long startpos=0);

		void replaceall(str &findsrc, char *replacesrc, long startpos=0);
		void replaceall(str &findsrc, wchar_t *replacesrc, long startpos=0);
		void replaceall(str &findsrc, unsigned long *replacesrc, long startpos=0);
		void replaceall(str &findsrc, unsigned short *replacesrc, long startpos=0);


		void setleft(char *src);

		void setleft(wchar_t *src);

		void setleft(unsigned long *src);

		void setleft(unsigned short *src);

		void setleft(str &strobj);
		
		void setright(str &strobj);

		void setright(char *src);

		void setright(wchar_t *src);

		void setright(unsigned long *src);

		void setright(unsigned short *src);

		void operator =(char *src)				;
		void operator =(wchar_t *src)			;
		void operator =(unsigned long *src)		;
		void operator =(unsigned short *src)	;
		void operator =(str &src)				;
		void operator =(const str &src)			;
		void operator =(int src)				;
		void operator =(unsigned int src)		;
		void operator =(long src)				;
		void operator =(unsigned long src)		;
		void operator =(float src)				;
		void operator =(double src)				;
	
		void operator +=(char *src)				;
		void operator +=(wchar_t *src)			;
		void operator +=(unsigned long *src)	;
		void operator +=(unsigned short *src)	;
		void operator +=(str &src)				;
		void operator +=(int src)				;
		void operator +=(unsigned int src)		;
		void operator +=(long src)				;
		void operator +=(unsigned long src)		;
		void operator +=(float src)				;
		void operator +=(double src)			;
		
		operator char *();
		operator unsigned short *();
		operator unsigned long *();
		operator wchar_t * ();

		bool operator ==(str &src);
		bool operator ==(char *src);
		bool operator ==(wchar_t *src);
		bool operator ==(unsigned long *src);
		bool operator ==(unsigned short *src);

		void dispose();
		~str();
	};

ns1end

#endif