#ifndef _AnmarLib_debugtools_H_
#define _AnmarLib_debugtools_H_

#include "lib.h"
#define _print libroot::print
#if defined(_debug)
#define _dprint _print
#else
#define _dprint libroot::donothing
#endif

mainns
	void setprinttag(char *ptag);
	void print(const char * _Format, ...);
	void donothing(const char * _Format, ...);
nsend

#endif