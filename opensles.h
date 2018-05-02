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

#ifndef _AnmarLib_opensles_H_
#define _AnmarLib_opensles_H_

/*
 * OpenSL ES class
 * Written by Anmar Hasan (anmarhasan@gmail.com)
 * Copyright 2013 by Anmar Hasan
 *
 * This is propriatary software and no one has permission to use, copy, sell, distribute or modify any part or all parts of this code.
 */
#if defined(_useandroid_)
#include "lib.h"
#include "general.h"
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

ns1(audio)
	class opensles
	{

		SLObjectItf engine, outputmix, player, recorder;
		SLEngineItf iengine;
		SLPlayItf iplayer;
		SLRecordItf irecorder;
		SLAndroidSimpleBufferQueueItf outbufferqueue;
		SLAndroidSimpleBufferQueueItf inbufferqueue;

		long currentoutbuffer;
		long currentinbuffer;
		long _totaloutbuffers;
		long _totalinbuffers;
		long _maxoutbuffersize;
		long _maxinbuffersize;
		libroot::general::VarDataArray _outputbuffers, _inputbuffers;

		void *_useroutcontext;
		void *_userincontext;
		void (*outputcallback)(opensles *contex, void *context);
		static void outcallbackstatic(SLAndroidSimpleBufferQueueItf buf, void *context);
		void outcallback(SLAndroidSimpleBufferQueueItf buf, void *context);

		void (*inputcallback)(opensles *contex, void *context);
		static void incallbackstatic(SLAndroidSimpleBufferQueueItf buf, void *context);
		void incallback(SLAndroidSimpleBufferQueueItf buf, void *context);

		void prepareoutbuffers();
		void prepareinbuffers();

		void opensldestroy(SLObjectItf obj);


		bool isinitialized_engine;
		bool isinitialized_output;
		bool isinitialized_input;

	public:

		opensles();


		SLresult initall(	void (*cbOut)(opensles *contex, void *context), 					// if audioOut is true, then supply here the output buffers callback function.
							void (*cbIn)(opensles *contex, void *context), 					// if audioIn is true, then supply here the input buffers callback function.
							int outbuffers,					// number of output buffers
							int inbuffers,					// number of input buffers
							long maxoutbuffersize,			// max size in bytes of each output buffer
							long maxinbuffersize,			// max size in bytes of each input buffer
							void *outcontext,				// user output context (pointer)
							void *incontext,				// user input context (pointer)
							long outsamplingrate=44100, 	// output sampling rate
							long insamplingrate=44100, 		// input sampling rate
							long outputchannels=2, 			// output channels
							long inputchannels=1, 			// input channels
							int outputbitspersample=16, 	// bits per sample for output
							int inputbitspersample=16);		// bits per sample for input

		SLresult initengine();
		SLresult initoutput(void (*cbOut)(opensles *thisopensles, void *context),int outbuffers,long maxoutbuffersize, void *outcontext, long outsamplingrate=44100, long outputchannels=1, int outputbitspersample=16);
		SLresult initinput(void (*cbIn)(opensles *thisopensles, void *context),int inbuffers,long maxinbuffersize, void *incontext, long insamplingrate=44100, long inputchannels=1, int inputbitspersample=16);

		void pushout(void *data, long size);
		void pushout(libroot::general::smartbuf &buf);
		void grabin(libroot::general::smartbuf &buf);

		SLresult startoutput();		// Starts playing.
		SLresult pauseoutput();		// Pauses playing.
		SLresult stopoutput();		// Stops playing.

		SLresult startinput();		// Starts recording.
		SLresult pauseinput();		// Pauses recording.
		SLresult stopinput();		// Stops recording.

		~opensles();

		void closeout();			// Closes output.
		void closein();				// Closes input.
		void close();				// Closes output, input and engine.

	};
ns1end
#endif

#endif