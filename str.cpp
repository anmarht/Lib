#pragma once
#include "lib.h"
#include "str.h"
#include <string.h>
#include <stdio.h>
ns1(general)

	void str::SetDefaults()
	{
		charsize=4;
	}

	void str::pushnull()
	{
		switch (charsize)
		{
		case 1:
			buf.push((char *)"\0",1);
			break;
		case 2:
			buf.push((char *)"\0\0",2);
			break;
		case 3:
			buf.push((char *)"\0\0\0",3);
			break;
		case 4:
			buf.push((char *)"\0\0\0\0",4);
			break;
		default:
			break;
		}
	}


	str::str()
	{
		SetDefaults();
	}

	int str::GetCharSize()
	{
		return charsize;
	}

	long str::Len()
	{
		return getlength();
	}
	void str::set(char* cstr)
	{
		buf.resize(0);
		add(cstr);
	}
	void str::set(wchar_t* wstr)
	{
		buf.resize(0);
		add(wstr);
	}
	void str::set(unsigned long* utf32str)
	{
		buf.resize(0);
		add(utf32str);
	}
	void str::set(unsigned short* utf16str)
	{
		buf.resize(0);
		add(utf16str);
	}
	void str::set(str &strobj)
	{
		buf.resize(0);
		add(strobj);
	}
	void str::set(const str &strobj)
	{
		buf.resize(0);
		add(strobj);
	}
	void str::set(int number)
	{
		buf.resize(0);
		add(number);
	}
	void str::set(unsigned int number)
	{
		buf.resize(0);
		add(number);
	}
	void str::set(long number)
	{
		buf.resize(0);
		add(number);
	}
	void str::set(unsigned long number)
	{
		buf.resize(0);
		add(number);
	}
	void str::set(float number)
	{
		buf.resize(0);
		add(number);
	}
	void str::set(double number)
	{
		buf.resize(0);
		add(number);
	}

	void str::add(char* cstr)
	{
		// Convert to wide chars
		if (cstr==NULL) return;			
		int reqlen=libroot::general::UTF8toUnicode_s(cstr,NULL,-1);
		tmp.resize(charsize*reqlen);	
		libroot::general::UTF8toUnicode_s(cstr,(unsigned long *)tmp.buffer,-1);
		buf.cropright(charsize);
		buf.push((char *)tmp.buffer,tmp.len);
		pushnull();		
	}	

	void str::add(wchar_t* wstr)
	{
		if (wstr==NULL) return;
		tmp.resize(0);
		#if defined(_useposix_)
			tmp.push((char *)wstr,libroot::general::getlenw(wstr)*4);
		#endif
		#if defined(_usewindows_)
			int reqlen=libroot::general::UTF16toUnicode_s((unsigned short *)wstr,NULL,-1);
			tmp.resize(charsize*reqlen);
			libroot::general::UTF16toUnicode_s((unsigned short *)wstr,(unsigned long *)tmp.buffer,-1);
		#endif
		buf.cropright(charsize);
		buf.push((char *)tmp.buffer,tmp.len);
		pushnull();		
	}
	void str::add(unsigned long* utf32str)
	{
		if (utf32str==NULL) return;
		tmp.push((char *)utf32str,libroot::general::getlenUTF32((char *)utf32str)*4);
		buf.cropright(charsize);
		buf.push((char *)tmp.buffer,tmp.len);
		pushnull();	
	}
	void str::add(unsigned short* utf16str)
	{
		if (utf16str==NULL) return;
		int reqlen=libroot::general::UTF16toUnicode_s(utf16str,NULL,-1);
		tmp.resize(charsize*reqlen);
		libroot::general::UTF16toUnicode_s(utf16str,(unsigned long *)tmp.buffer,-1);
		buf.cropright(charsize);
		buf.push((char *)tmp.buffer,tmp.len);
		pushnull();	
	}
	void str::add(const str &strobj)
	{
		add((unsigned long *)strobj.buf.buffer);
	}
	void str::add(str &strobj)
	{
		add((unsigned long *)strobj.buf.buffer);
	}

	void str::add(int number)
	{
		add ((long)number);
	}
	void str::add(unsigned int number)
	{
		add ((unsigned long)number);
	}
	void str::add(long number)
	{
		char db[20];
		sprintf(db,"%d",number);
		add(db);
	}
	void str::add(unsigned long number)
	{
		char db[20];
		sprintf(db,"%lu",number);
		add(db);
	}
	void str::addhex(int number, int padding)
	{
		addhex((long)number,padding);
	}
	void str::addhex(unsigned int number, int padding)
	{
		addhex((unsigned long)number, padding);
	}
	void str::addhex(long number, int padding)
	{
		char db[20];
		sprintf(db,"%.*x",padding,number);
		add(db);
	}
	void str::addhex(unsigned long number, int padding)
	{
		char db[20];
		sprintf(db,"%.*x",padding,number);
		add(db);
	}

	void str::add(float number)
	{
		char db[20];
		sprintf(db,"%f",number);
		add(db);
	}

	void str::add(double number)
	{
		char db[20];
		sprintf(db,"%f",number);
		add(db);
	}
	long str::getlength()
	{	long l;
		l=buf.len/charsize;
		if (l==0) return 0;
		return l-1;
	}
	long str::getsizebytes()
	{
		return buf.len;
	}

	void str::cropleft(long count)
	{	if (count<=0) return;
		if (count>=getlength()) {set("");return;}
		buf.cropleft(count*charsize);
			
	}
	void str::cropright(long count)
	{	if (count<=0) return;
		if (count>=getlength()) {set("");return;}
		buf.cropright((count+1)*charsize);
		pushnull();
	}

	void str::cropmid(long start, long count)
	{	long e,ol;
		if ((count<=0)||(start<0)) return;
		if (start>getlength()) return;
		e=start+count;ol=getlength();
		buf.deleteat(start*charsize,count*charsize);
		if (e>ol) pushnull();
	}
	
	void str::setat(long index, wchar_t *src)
	{
		str t;
		t.set(src);		
		buf.updatebuffer(t.buf.buffer,t.buf.len,index*charsize);
	}
	void str::setat(long index, char *src)
	{
		str t;
		t.set(src);		
		buf.updatebuffer(t.buf.buffer,t.buf.len,index*charsize);
	}
	void str::setat(long index, unsigned long *src)
	{
		str t;
		t.set(src);		
		buf.updatebuffer(t.buf.buffer,t.buf.len,index*charsize);
	}
	void str::setat(long index, unsigned short *src)
	{
		str t;
		t.set(src);		
		buf.updatebuffer(t.buf.buffer,t.buf.len,index*charsize);
	}

	void str::setat(long index, str &src)
	{		
		buf.updatebuffer(src.buf.buffer,src.buf.len,index*charsize);
	}
	char *str::getutf8()
	{		
		int reqlen=libroot::general::UnicodetoUTF8_s((unsigned long *)buf.buffer,-1,NULL);
		tmp.resize(reqlen);
		libroot::general::UnicodetoUTF8_s((unsigned long *)buf.buffer,-1,(char *)tmp.buffer);
		tmp.push("\0",1);
		return (char *)tmp.buffer;
	}

	unsigned short *str::getutf16()
	{
		int reqlen=libroot::general::UnicodetoUTF16_s((unsigned long *)buf.buffer,-1,NULL);
		tmp.resize(reqlen*2);
		libroot::general::UnicodetoUTF16_s((unsigned long *)buf.buffer,-1,(unsigned short *)tmp.buffer);
		tmp.push("\0\0",2);
		return (unsigned short *)tmp.buffer;
	}
	unsigned long *str::getutf32()
	{
		return (unsigned long*)buf.buffer;
	}

	wchar_t *str::getwide()
	{
		#if defined(_useposix_)
		return (wchar_t *)getutf32();
		#endif
		#if defined(_usewindows_)
		return (wchar_t *)getutf16();
		#endif
	}

	void str::insertat(long index, char *src)
	{
		str tmp; tmp.set(src);
		buf.pushin(tmp.buf.buffer,index*charsize,tmp.getlength()*charsize);
	}
	void str::insertat(long index, wchar_t *src)
	{
		str tmp; tmp.set(src);
		buf.pushin(tmp.buf.buffer,index*charsize,tmp.getlength()*charsize);
	}
	void str::insertat(long index, unsigned long *src)
	{
		str tmp; tmp.set(src);
		buf.pushin(tmp.buf.buffer,index*charsize,tmp.getlength()*charsize);
	}
	void str::insertat(long index, unsigned short *src)
	{
		str tmp; tmp.set(src);
		buf.pushin(tmp.buf.buffer,index*charsize,tmp.getlength()*charsize);
	}
	void str::insertat(long index, str &src)
	{
		str tmp; tmp.set(src);
		buf.pushin(tmp.buf.buffer,index*charsize,tmp.getlength()*charsize);
	}
	bool str::isequalto(str &strobj)
	{
		long i;
		long length=getlength();
		if (length!=strobj.getlength()) return FALSE;

		if (memcmp((void *)buf.buffer, (void *)strobj.buf.buffer,length*charsize)==0)
			return TRUE;
		return FALSE;
	}
	bool str::isequalto(char *src)
	{
		str tmp; tmp.set(src);
		return isequalto(tmp);
	}
	bool str::isequalto(wchar_t *src)
	{
		str tmp; tmp.set(src);
		return isequalto(tmp);
	}
	bool str::isequalto(unsigned long *src)
	{
		str tmp; tmp.set(src);
		return isequalto(tmp);
	}	
	bool str::isequalto(unsigned short *src)
	{
		str tmp; tmp.set(src);
		return isequalto(tmp);
	}

	void str::mid(long start, long count, str &targetstr)
	{
		if ((start<0)||(start>getlength())) return;
		long length=getlength();
		long mycount=count;
		if ((start+count)>length) mycount=length-start;
		libroot::general::smartbuf tmp;
		tmp.push((char *)buf.buffer+start*charsize,mycount*charsize);
		tmp.push((char *)"\0\0\0\0",4);
		targetstr.set((unsigned long *)tmp.buffer);
	}
	
	void str::left(long count, str &targetstr)
	{
		return mid(0,count,targetstr);
	}
	void str::right(long count, str &targetstr)
	{
		return mid(getlength()-count,count,targetstr);
	}

	long str::instr(str &findstr, long startpos, int totaliterations)
	{
		long length=getlength();
		if ((startpos<0)||(startpos>length)) return -1;
		long findstrlen=findstr.getlength();
		if (findstrlen==0) return -1;
		if (findstrlen>length) return -1;

		long i, iterationcountdown;
		iterationcountdown=totaliterations;
		for (i=startpos;i<=(length-findstrlen);i++)
		{
			bool found=FALSE;
			if (memcmp((void *)((unsigned long *)((char *)buf.buffer+i*4)), (void *)((unsigned long *)findstr.buf.buffer),findstrlen*charsize)==0)
				found=TRUE;
			if (found)
			{
				iterationcountdown--;
				if (iterationcountdown==0) return i;
			}
		}
		return -1;
	}

	long str::instr(char *src, long startpos, int totaliterations)
	{
		str tmp; tmp.set(src);
		return instr(tmp,startpos,totaliterations);
	}
	long str::instr(wchar_t *src, long startpos, int totaliterations)
	{
		str tmp; tmp.set(src);
		return instr(tmp,startpos,totaliterations);
	}
	long str::instr(unsigned long *src, long startpos, int totaliterations)
	{
		str tmp; tmp.set(src);
		return instr(tmp,startpos,totaliterations);
	}
	long str::instr(unsigned short *src, long startpos, int totaliterations)
	{
		str tmp; tmp.set(src);
		return instr(tmp,startpos,totaliterations);
	}

	bool str::contains(str strobj)
	{
		if (instr(strobj,0)>=0) return true;
		return false;
	}
	bool str::contains(char * strsrc)
	{
		if (instr(strsrc,0)>=0) return true;
		return false;
	}
	bool str::contains(wchar_t * strsrc)
	{
		if (instr(strsrc,0)>=0) return true;
		return false;
	}
	bool str::contains(unsigned long * strsrc)
	{
		if (instr(strsrc,0)>=0) return true;
		return false;
	}
	bool str::contains(unsigned short * strsrc)
	{
		if (instr(strsrc,0)>=0) return true;
		return false;
	}


	void str::replace(str &findstr, str &replacestr, long startpos)
	{
		long ind=instr(findstr,startpos);
		if (ind<0) return;
		cropmid(ind,findstr.getlength());
		insertat(ind,replacestr);
	}

	void str::replaceall(str &findstr, str &replacestr, long startpos)
	{
		long ind;
		long findstrlen=findstr.getlength();
		while ((ind=instr(findstr,0))>=0)
		{
			cropmid(ind,findstrlen);
			insertat(ind,replacestr);
		}
	}

	void str::replace(char *findsrc, char *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}
	void str::replace(char *findsrc, wchar_t *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}
	void str::replace(char *findsrc, unsigned long *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}
	void str::replace(char *findsrc, unsigned short *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}
	void str::replace(char *findsrc,  str &replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}

	void str::replace(wchar_t *findsrc, char *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}
	void str::replace(wchar_t *findsrc, wchar_t *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}
	void str::replace(wchar_t *findsrc, unsigned long *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}
	void str::replace(wchar_t *findsrc, unsigned short *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}
	void str::replace(wchar_t *findsrc,  str &replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}

	void str::replace(unsigned long *findsrc, char *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}
	void str::replace(unsigned long *findsrc, wchar_t *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}
	void str::replace(unsigned long *findsrc, unsigned long *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}
	void str::replace(unsigned long *findsrc, unsigned short *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}
	void str::replace(unsigned long *findsrc,  str &replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}

	void str::replace(unsigned short *findsrc, char *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}
	void str::replace(unsigned short *findsrc, wchar_t *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}
	void str::replace(unsigned short *findsrc, unsigned long *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}
	void str::replace(unsigned short *findsrc, unsigned short *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}
	void str::replace(unsigned short *findsrc,  str &replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}

	void str::replace(str &findsrc, char *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}
	void str::replace(str &findsrc, wchar_t *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}
	void str::replace(str &findsrc, unsigned long *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}
	void str::replace(str &findsrc, unsigned short *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replace(tmpf, tmpr, startpos);
	}

	void str::replaceall(char *findsrc, char *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}
	void str::replaceall(char *findsrc, wchar_t *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}
	void str::replaceall(char *findsrc, unsigned long *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}
	void str::replaceall(char *findsrc, unsigned short *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}
	void str::replaceall(char *findsrc,  str &replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}

	void str::replaceall(wchar_t *findsrc, char *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}
	void str::replaceall(wchar_t *findsrc, wchar_t *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}
	void str::replaceall(wchar_t *findsrc, unsigned long *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}
	void str::replaceall(wchar_t *findsrc, unsigned short *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}
	void str::replaceall(wchar_t *findsrc,  str &replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}

	void str::replaceall(unsigned long *findsrc, char *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}
	void str::replaceall(unsigned long *findsrc, wchar_t *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}
	void str::replaceall(unsigned long *findsrc, unsigned long *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}
	void str::replaceall(unsigned long *findsrc, unsigned short *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}
	void str::replaceall(unsigned long *findsrc,  str &replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}

	void str::replaceall(unsigned short *findsrc, char *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}
	void str::replaceall(unsigned short *findsrc, wchar_t *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}
	void str::replaceall(unsigned short *findsrc, unsigned long *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}
	void str::replaceall(unsigned short *findsrc, unsigned short *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}
	void str::replaceall(unsigned short *findsrc,  str &replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}

	void str::replaceall(str &findsrc, char *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}
	void str::replaceall(str &findsrc, wchar_t *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}
	void str::replaceall(str &findsrc, unsigned long *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}
	void str::replaceall(str &findsrc, unsigned short *replacesrc, long startpos)
	{
		str tmpf, tmpr;
		tmpf.set(findsrc);tmpr.set(replacesrc);
		replaceall(tmpf, tmpr, startpos);
	}


	void str::setleft(char *src)
	{
		setat(0,src);
	}
	void str::setleft(wchar_t *src)
	{
		setat(0,src);
	}
	void str::setleft(unsigned long *src)
	{
		setat(0,src);
	}
	void str::setleft(unsigned short *src)
	{
		setat(0,src);
	}
	void str::setleft(str &strobj)
	{
		setat(0,strobj);
	}

	void str::setright(str &strobj)
	{
		long strobjlen=strobj.getlength();
		long length=getlength();
		if (strobjlen>=length)
		{
			setat(0,strobj);
			return;
		}

		long start=length-strobjlen;
		setat(start,strobj);
	}

	void str::setright(char *src)
	{
		str tmp;tmp.set(src);
		setright(tmp);
	}
	void str::setright(wchar_t *src)
	{
		str tmp;tmp.set(src);
		setright(tmp);
	}
	void str::setright(unsigned long *src)
	{
		str tmp;tmp.set(src);
		setright(tmp);
	}
	void str::setright(unsigned short *src)
	{
		str tmp;tmp.set(src);
		setright(tmp);
	}

	
	void str::operator =(char *src)				{set(src);}
	void str::operator =(wchar_t *src)			{set(src);}
	void str::operator =(unsigned long *src)		{set(src);}
	void str::operator =(unsigned short *src)	{set(src);}
	void str::operator =(str &src)				{set(src);}
	void str::operator =(const str &src)			{set(src);}
	void str::operator =(int src)				{set(src);}
	void str::operator =(unsigned int src)		{set(src);}
	void str::operator =(long src)				{set(src);}
	void str::operator =(unsigned long src)		{set(src);}
	void str::operator =(float src)				{set(src);}
	void str::operator =(double src)				{set(src);}
	
	void str::operator +=(char *src)				{add(src);}
	void str::operator +=(wchar_t *src)			{add(src);}
	void str::operator +=(unsigned long *src)	{add(src);}
	void str::operator +=(unsigned short *src)	{add(src);}
	void str::operator +=(str &src)				{add(src);}
	void str::operator +=(int src)				{add(src);}
	void str::operator +=(unsigned int src)		{add(src);}
	void str::operator +=(long src)				{add(src);}
	void str::operator +=(unsigned long src)		{add(src);}
	void str::operator +=(float src)				{add(src);}
	void str::operator +=(double src)			{add(src);}

	str::operator char *()			{return getutf8();}
	str::operator unsigned short *() {return getutf16();}
	str::operator unsigned long *()	{return getutf32();}
	str::operator wchar_t * ()		{return getwide();}

	bool str::operator ==(str &src) 				{return isequalto(src);}
	bool str::operator ==(char *src) 			{return isequalto(src);}
	bool str::operator ==(wchar_t *src) 			{return isequalto(src);}
	bool str::operator ==(unsigned long *src) 	{return isequalto(src);}
	bool str::operator ==(unsigned short *src) 	{return isequalto(src);}

	void str::dispose()
	{
		tmp.dispose();
		buf.dispose();
	}

	str::~str()
	{
		dispose();
	}

ns1end 