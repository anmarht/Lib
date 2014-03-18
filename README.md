AHCLIBCP: Crossplatform library
===

Contains a number of classes and functions covering memory management, strings, encoding, networking, Android's OPENSLES, threading, events.

I've converted this subset from the my 2008 Windows library and made it cross platform.

Tested on Windows, Linux (RHEL6), Mac OS 10.9, Android 4, iOS 7.

Classes that will be added later: 
- Wrappers for iOS Remote IO audio unit recording/playback.   - Undergoing testing.
- Wrappers for Mac audio i/o recording/playback.              - Undergoing testing.
- Wrappers for iOS/Mac AudioConverter.                        - Undergoing testing.
- Wavein/out wrappers for Windows.                            - Tested and ready

Future to do:
- Unified interface for audio recording/playback for Windows, Android and Mac/iOS.


Missing:

- Documentation
- Command line build scripts.
- License
