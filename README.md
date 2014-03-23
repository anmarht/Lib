AHCLIBCP: Crossplatform library
===

Contains a number of classes and functions covering memory management, strings, encoding, networking, Android's OPENSLES, threading, events.

I've converted this subset from the my 2008 Windows library and made it cross platform.

Tested on Windows, Linux (RHEL6), Mac OS 10.9, Android 4, iOS 7.

Before compiling
-----

Edit the lib.h file and modify the #define to match the target platform:

for Windows, use #define _usewindows_
for Linux, use #define _useposix_
for Android, use #define _useposix_ and #define _useandroid_
for mac os, use #define _useposix_
for iOS, use #define _useposix_ and #define _useios_

------------

- Windows:

	* Create a project using Visual Studio.

	* Add library files to the project.

	* Compile and build.
	
- Mac OS / iOS

	* Create a project using XCode.

	* Add library files to the project.

	* Compile and build.

- Linux: I've used Eclipse to compile and build.

	* Create a project using Eclipse.

	* Add library files to the project.

	* Compile and build.
	
Sample Android make file:

	LOCAL_PATH := $(call my-dir)

	include $(CLEAR_VARS)

	LOCAL_MODULE    := mylib
	LOCAL_SRC_FILES :=  general.cpp 		\
						textencoding.cpp 	\
						str.cpp 			\
						helpers.cpp 		\
						netgeneral.cpp 		\
						dns.cpp 			\
						debugtools.cpp 		\
						asocket.cpp 		\
						thread.cpp			\
						sync.cpp			\
						md5.cpp				\
						PacketBase.cpp		\
						LightPacket.cpp		\
						PacketEngine.cpp	\
						opensles.cpp		\
						lanbroadcaster.cpp	

	LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -llog -lOpenSLES -ljnigraphics

	include $(BUILD_SHARED_LIBRARY)



Classes that will be added later: 
---
- Wrappers for iOS Remote IO audio unit recording/playback.   - Undergoing testing.
- Wrappers for Mac audio i/o recording/playback.              - Undergoing testing.
- Wrappers for iOS/Mac AudioConverter.                        - Undergoing testing.
- Wavein/out wrappers for Windows.                            - Tested and ready

Future to do:
---
- Unified interface for audio recording/playback for Windows, Android and Mac/iOS.

Missing:
---
- Documentation
