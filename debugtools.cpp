#include "lib.h"
#include "helpers.h"
#if defined(_usewindows_) || (defined(_useposix_) && !defined(_useandroid_))
#include <stdio.h>
#include <string.h>
#endif
#include "general.h"
#include <stdarg.h>
#if defined(_useandroid_)
	#include <android/log.h>
	#include <stdio.h>
	#include <string.h>
#endif

mainns
	char _printtag[65]="\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";

	void setprinttag(char *ptag)
	{
		strcpy(_printtag,ptag);
	}
	void print(const char * _Format, ...)
	{
		#if defined(_usewindows_) || (defined(_useposix_) && !defined(_useandroid_))
			va_list args;
			va_start(args,_Format);
			vprintf(_Format,args);
			va_end(args);
		#endif
		#if defined(_useandroid_)
			va_list args;
			va_start(args,_Format);
			__android_log_vprint(ANDROID_LOG_INFO, _printtag, _Format, args);
			va_end(args);

		#endif
	}
	void donothing(const char * _Format, ...)
	{

	}
nsend
