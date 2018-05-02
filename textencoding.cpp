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
#include "textencoding.h"

#ifndef byte
#define byte unsigned char
#endif

ns1 (general)

	// returns number of bytes for the utf-8 sequence, supports up to 4 bytes of UTF8, 
	// returns number of bytes required if utf8dst=NULL,
	// returns -1 if require more than 4 bytes.
	int UnicodetoUTF8(unsigned long Unicodevalue, char *utf8dst)
	{
		char *dst=utf8dst;
		unsigned char utf8_1, utf8_2, utf8_3, utf8_4;
        
        Unicodevalue=Unicodevalue & 0xffffffff;
        
		if (Unicodevalue<0x80)
		{
			if (utf8dst==NULL) return 1;
			utf8_1=(char)Unicodevalue;
			*dst=utf8_1;
			return 1;
		}

		if (Unicodevalue<0x800)
		{
			if (utf8dst==NULL) return 2;
			unsigned long v=Unicodevalue << 2;

			byte *pU=(byte*)&v;
			byte ub1=(*pU) >> 2, ub2= *(pU+1);
		
			utf8_1=0xc0 | ub2;
			utf8_2=0x80 | ub1;

			*dst=utf8_1;dst++;
			*dst=utf8_2;
			return 2;
		}

		if (Unicodevalue<0x10000)
		{
			if (utf8dst==NULL) return 3;
			byte *pU=(byte*)&Unicodevalue;
			byte ub1=(*pU), ub2= *(pU+1);
		
			utf8_1=0xe0 | (ub2 >> 4);
			utf8_2=0x80 | ((ub2 & 0xf) <<2) | (ub1 >> 6);
			utf8_3=0x80 | (ub1 & 0x3f);

			*dst=utf8_1;dst++;
			*dst=utf8_2;dst++;
			*dst=utf8_3;
			return 3;
		}

		if (Unicodevalue<0x200000)
		{
			if (utf8dst==NULL) return 4;
			unsigned long v=Unicodevalue;
			byte *pU=(byte*)&v;
			byte ub1=(*pU), ub2= *(pU+1), ub3= *(pU+2);
		
			utf8_1=0xf0 | (ub3 >> 2);
			utf8_2=0x80 | ((ub3 & 3) << 4) | (ub2 >> 4);
			utf8_3=0x80 | ((ub2 & 0xf) << 2) | ( ub1 >> 6);
			utf8_4=0x80 | (ub1 & 0x3f);

			*dst=utf8_1;dst++;
			*dst=utf8_2;dst++;
			*dst=utf8_3;dst++;
			*dst=utf8_4;
			return 4;
		}
		if (Unicodevalue>=0x200000)
		{
			return -1;
		}
        return 0;
	}


	// Converts UTF8 multibyte character to Unicode, supports up to 4 bytes of UTF8.
	// returns the number of bytes this character is composed of.
	// the unicode character output is in the outunicode parameter.
	// returns -1 if the utf8 is for a unicode of more than 4 bytes.
	long UTF8toUnicode(char *utf8MB, unsigned long &outunicode)
	{
		byte first=(byte)*utf8MB;
		outunicode=0;
		if ((first >> 7) == 0) 
		{
			outunicode=(unsigned long) first;
			return 1;
		}
		if ((first >> 5) == 6)
		{
			byte second=(byte) *(utf8MB+1);
			outunicode = ((first & 0x1f) << 6) | ((first & 3) << 6) | (second & 0x3f);
			return 2;
		}
		if ((first >> 4) == 0xe)
		{
			byte second=(byte) *(utf8MB+1);
			byte third=(byte) *(utf8MB+2);
			outunicode =  (third & 0x3f) | ((second & 3) << 6) | ((second & 0x3c) << 6) | ((first & 0xf) << 12);
			return 3;
		}
		if ((first >> 3) == 0x1e)
		{
			byte second=(byte) *(utf8MB+1);
			byte third=(byte) *(utf8MB+2);
			byte fourth=(byte) *(utf8MB+3);

			outunicode = (fourth & 0x3f) | ((third & 3) << 6) | ((third & 0x3c) << 6) | ((second & 0xf) << 12) | ((second & 0x30) << 12) | ((first & 7) << 18);

			return 4;
		}
		return -1;
	}


	// converts a Unicode (32-bit character) string to UTF-8, returns total number of bytes of the UTF-8 multibyte string,
	// returns required length if utf8dst=NULL;
	// if unicodecounttoconvert=-1, then the function assumes null terminated unicode (4 bytes a character) source string.
	// if unicodestr=NULL the function returns -1.
	long UnicodetoUTF8_s(unsigned long *unicodestr, long unicodecounttoconvert, char *utf8dst)
	{
		if (unicodestr==NULL) return -1;
	
		unsigned long *pu=unicodestr;
		char *dst=utf8dst;

		long count=0, totallength=0;
		while (((unicodecounttoconvert>0)&&(count<unicodecounttoconvert))||unicodecounttoconvert==-1)
		{ 
			pu=(unsigned long *)((char *)unicodestr+4*count);
			if ((*pu==0)&&(unicodecounttoconvert==-1)) break;	
			int len=UnicodetoUTF8(*pu, dst);
			if (len==-1) break;
			totallength+=len;
			if (dst!=NULL) dst+=len;
			count++;
		}

		return totallength;
	}


	// Converts UTF8 multibyte string to Unicode string (4 bytes a character).
	// If unicodemaxcount=-1 the function assumes a Null terminated UTF8 string.
	// if unicodestrdst=NULL the function returns total count of characters required in the Unicode string (each character is 4 bytes).
	long UTF8toUnicode_s(char *utf8str, unsigned long *unicodestrdst, long unicodemaxcount)
	{
		if (utf8str==NULL) return -1;
	
		char *putf8=utf8str;
		unsigned long  *dst=unicodestrdst;
		short tmp, *ptmp;
		long count=0;
		unsigned long unicode;
		while (((unicodemaxcount>0)&&(count<unicodemaxcount))||unicodemaxcount==-1)
		{
			int len=UTF8toUnicode(putf8, unicode);	
			if (unicode==0) break;
			if (len==-1) break;
			if (dst!=NULL) {dst=(unsigned long *)((char *)unicodestrdst+4*count); 
			ptmp=(short *)dst;
			tmp=unicode & 0xffff;
			*ptmp=tmp;
			ptmp++;
			tmp=(unicode >> 16) & 0xffff;
			*ptmp=tmp;
			}
			putf8+=len;count++;
		}

		return count;
	}


	// returns number of words (word=2 bytes) for the utf-16 sequence. 
	// returns number of bytes required if utf16dst=NULL,
	// returns -1 if require more than 4 bytes.
	int UnicodetoUTF16(unsigned long Unicodevalue, unsigned short *utf16dst)
	{
		unsigned short *dst=utf16dst;
		unsigned short utf16_1, utf16_2;
		if (Unicodevalue<=0xffff)
		{
			if (utf16dst==NULL) return 1;
			utf16_1=(unsigned short)Unicodevalue;
			*dst=utf16_1;
			return 1;
		}
		if (Unicodevalue>0xffff)
		{
			if (utf16dst==NULL) return 2;		
			unsigned long tmp=Unicodevalue - 0x10000;
			unsigned short leadsurrogate= 0xD800 + (tmp >> 10);
			unsigned short trailsurrogate= 0xDC00 + (tmp & 0x3ff);

			utf16_1=leadsurrogate;
			utf16_2=trailsurrogate;
			*dst=utf16_1;dst++;
			*dst=utf16_2;

			return 2;
		}
        return 0;
	}


	// Converts UTF16 multiword character to Unicode, supports up to 4 bytes of UTF8.
	// returns the number of words this UTF16 character was composed of.
	// the unicode character output is in the outunicode parameter.
	// returns -1 if utf16MW=NULL.
	int UTF16toUnicode(unsigned short *utf16MW, unsigned long &outunicode)
	{
		unsigned short first=(unsigned short)*utf16MW;
		outunicode=0;
		if (utf16MW==NULL) return -1;
		if ((first >=0xD800)&&(first <=0xDBFF)) // this is a pair of words
		{
			unsigned short trail=(unsigned short)*(utf16MW+1);
		
			outunicode= (((first -0xd800) << 10 ) | ((trail - 0xdc00) &0x3ff))+ 0x10000;

			return 2;
		}
		else		// this is a one word
		{
			outunicode= (unsigned long)first;
			return 1;
		}
	}


	// converts a Unicode (32-bit character) string to UTF-16, returns total number of bytes of the UTF-16 multibyte string,
	// returns required length (in words) if utf16dst=NULL or the total number of words converted.
	// if unicodecounttoconvert=-1, then the function assumes null terminated unicode (4 bytes a character) source string.
	// if unicodestr=NULL the function returns -1.
	long UnicodetoUTF16_s(unsigned long *unicodestr, long unicodecounttoconvert, unsigned short *utf16dst)
	{
		if (unicodestr==NULL) return -1;
	
		unsigned long *pu=unicodestr;
		unsigned short *dst=utf16dst;

		long count=0, totallength=0;
		while (((unicodecounttoconvert>0)&&(count<unicodecounttoconvert))||unicodecounttoconvert==-1)
		{
			if ((*pu==0)&&(unicodecounttoconvert==-1)) break;	
			int len=UnicodetoUTF16(*pu, dst);
			if (len==-1) break;
			totallength+=len;
			if (dst!=NULL) dst+=len;
			pu++;count++;
		}

		return totallength;
	}

	// Converts UTF16 multibyte string to Unicode string (4 bytes a character).
	// If unicodemaxcount=-1 the function assumes a Null terminated UTF8 string.
	// if unicodestrdst=NULL the function returns total count of characters required in the Unicode string (each character is 4 bytes).
	long UTF16toUnicode_s(unsigned short *utf16str, unsigned long *unicodestrdst, long unicodemaxcount)
	{
		if (utf16str==NULL) return -1;
	
		unsigned short *putf16=utf16str;
		unsigned long  *dst=unicodestrdst;

		long count=0;
		unsigned long unicode;
		while (((unicodemaxcount>0)&&(count<unicodemaxcount))||unicodemaxcount==-1)
		{
			int len=UTF16toUnicode(putf16, unicode);	
			if (unicode==0) break;
			if (len==-1) break;
			if (dst!=NULL) {*dst=unicode;dst++;}
			putf16+=len;count++;
		}

		return count;
	}

ns1end