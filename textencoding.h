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

#ifndef _AnmarLib_textencoding_H_
#define _AnmarLib_textencoding_H_

#include "lib.h"

ns1 (general)

	// returns number of bytes for the utf-8 sequence, supports up to 4 bytes of UTF8, 
	// returns number of bytes required if utf8dst=NULL,
	// returns -1 if require more than 4 bytes.
	int UnicodetoUTF8(unsigned long Unicodevalue, char *utf8dst);	

	// Converts UTF8 multibyte character to Unicode, supports up to 4 bytes of UTF8.
	// returns the number of bytes this character is composed of.
	// the unicode character output is in the outunicode parameter.
	// returns -1 if the utf8 is for a unicode of more than 4 bytes.
	long UTF8toUnicode(char *utf8MB, unsigned long &outunicode);

	// converts a Unicode (32-bit character) string to UTF-8, returns total number of bytes of the UTF-8 multibyte string,
	// returns required length if utf8dst=NULL;
	// if unicodecounttoconvert=-1, then the function assumes null terminated unicode (4 bytes a character) source string.
	// if unicodestr=NULL the function returns -1.
	long UnicodetoUTF8_s(unsigned long *unicodestr, long unicodecounttoconvert, char *utf8dst);

	// Converts UTF8 multibyte string to Unicode string (4 bytes a character).
	// If unicodemaxcount=-1 the function assumes a Null terminated UTF8 string.
	// if unicodestrdst=NULL the function returns total count of characters required in the Unicode string (each character is 4 bytes).
	long UTF8toUnicode_s(char *utf8str, unsigned long *unicodestrdst, long unicodemaxcount);

	// returns number of words (word=2 bytes) for the utf-16 sequence. 
	// returns number of bytes required if utf16dst=NULL,
	// returns -1 if require more than 4 bytes.
	int UnicodetoUTF16(unsigned long Unicodevalue, unsigned short *utf16dst);

	// Converts UTF16 multiword character to Unicode, supports up to 4 bytes of UTF8.
	// returns the number of words this UTF16 character was composed of.
	// the unicode character output is in the outunicode parameter.
	// returns -1 if utf16MW=NULL.
	int UTF16toUnicode(unsigned short *utf16MW, unsigned long &outunicode);
	
	// converts a Unicode (32-bit character) string to UTF-8, returns total number of bytes of the UTF-16 multibyte string,
	// returns required length (in words) if utf16dst=NULL or the total number of words converted.
	// if unicodecounttoconvert=-1, then the function assumes null terminated unicode (4 bytes a character) source string.
	// if unicodestr=NULL the function returns -1.
	long UnicodetoUTF16_s(unsigned long *unicodestr, long unicodecounttoconvert, unsigned short *utf16dst);
	
	// Converts UTF16 multibyte string to Unicode string (4 bytes a character).
	// If unicodemaxcount=-1 the function assumes a Null terminated UTF8 string.
	// if unicodestrdst=NULL the function returns total count of characters required in the Unicode string (each character is 4 bytes).
	long UTF16toUnicode_s(unsigned short *utf16str, unsigned long *unicodestrdst, long unicodemaxcount);
	
ns1end

#endif