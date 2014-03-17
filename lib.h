#pragma once

// Namespace macro definitions
#define libroot alib
#define mainns \
	namespace libroot {
#define nsend }
#define ns1(x1) \
	mainns \
		namespace x1 { 
#define ns1end }}

#define ns2(x1,x2) \
	mainns \
		namespace x1 { \
			namespace x2 { 
#define ns2end }}}

// Debugging: uncomment the line below to enable debug prints
// #define _debug

// Please define below what system to use, the available are:
// #define _usewindows_
// #define _useposix_
// #define _useandroid_

#define _useposix_
#define _useios_

// Disabling CRT warnings.
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif


#if defined(_useposix_) 
	#include "lib.posix.h"
#endif

#if defined(_usewindows_) 
	#include "lib.windows.h"
#endif

                

