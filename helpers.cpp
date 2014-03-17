#include "lib.h"
#include "helpers.h"
#if defined(_usewindows_) || (defined(_useposix_) && !defined(_useandroid_))
#include <stdio.h>
#endif

#if defined(_useandroid_)
	#include <android/log.h>
#endif

ns1(helpers)

void consolelog(char *tag, char *txt)
{
	#if defined(_usewindows_) || (defined(_useposix_) && !defined(_useandroid_))
		printf("\n%s: %s",tag,txt);
	#endif
	#if defined(_useandroid_)
		__android_log_write(ANDROID_LOG_INFO, tag, txt);
	#endif

}

ns1end