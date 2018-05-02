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
#include "opensles.h"
#include "debugtools.h"
#include "string.h"
#if defined(_useandroid_)
#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
ns1(audio)
	opensles::opensles()
	{
		isinitialized_engine=false;
		isinitialized_output=false;
		isinitialized_input=false;
		iplayer=NULL;
		irecorder=NULL;
		iengine=NULL;
	}
	opensles::~opensles()
	{
		close();
		_inputbuffers.DeleteAllItems();
		_outputbuffers.DeleteAllItems();
	}
	SLresult opensles::initall(	void (*cbOut)(opensles *contex, void *context), 					// if audioOut is true, then supply here the output buffers callback function.
								void (*cbIn)(opensles *contex, void *context), 					// if audioIn is true, then supply here the input buffers callback function.
								int outbuffers,					// number of output buffers
								int inbuffers,					// number of input buffers
								long maxoutbuffersize,			// max size in bytes of each output buffer
								long maxinbuffersize,			// max size in bytes of each input buffer
								void *outcontext,				// user context (pointer)
								void *incontext,				// user context (pointer)
								long outsamplingrate, 	// output sampling rate
								long insamplingrate, 		// input sampling rate
								long outputchannels, 			// output channels
								long inputchannels, 			// input channels
								int outputbitspersample, 	// bits per sample for output
								int inputbitspersample)		// bits per sample for input
	{
		SLresult rv;

		rv=initoutput(cbOut, outbuffers, maxoutbuffersize, outcontext, outsamplingrate, outputchannels, outputbitspersample);
		if (rv!=0) return rv;

		rv=initinput(cbIn, inbuffers, maxinbuffersize, incontext, insamplingrate, inputchannels, inputbitspersample);
		if (rv!=0) return rv;

		return 0;		// success
	}

	SLresult opensles::initengine()
	{
		if (isinitialized_engine) return 0;

		SLresult rv;
		// engine
		rv = slCreateEngine(&engine, 0, NULL, 0, NULL, NULL);	_print("slCreateEngine rv=%d",rv);
		if (rv!=0) return rv;
		rv = (*engine)->Realize(engine, SL_BOOLEAN_FALSE);	_print("engine realize rv=%d",rv);
		if (rv!=0) return rv;
		rv = (*engine)->GetInterface(engine, SL_IID_ENGINE, &iengine);	_print("engine get interface rv=%d",rv);
		if (rv!=0) return rv;

		isinitialized_engine=true;
		return 0;
	}

	SLresult opensles::initoutput(void (*cbOut)(opensles *thisopensles, void *context),int outbuffers,long maxoutbuffersize, void *outcontext, long outsamplingrate, long outputchannels, int outputbitspersample)
	{
		SLresult rv;
		rv=initengine();
		if (rv!=0) return rv;

		if (isinitialized_output)
		{
			stopoutput();
			closeout();
		}

		if (iengine==NULL) return -1;
		if (iplayer!=NULL) return -1;

		if ((cbOut==NULL)||(outbuffers==0)||(maxoutbuffersize==0)) return -1;

		if ((outsamplingrate==0)||(outputchannels==0)) return -1;

		if ((outputbitspersample % 8) >0) return -1;

		outputcallback=cbOut;

		_totaloutbuffers=outbuffers;
		_useroutcontext=outcontext;
		_maxoutbuffersize=maxoutbuffersize;
		currentoutbuffer=0;
		iplayer=0;
		// output mix
		const SLInterfaceID omids[] = {SL_IID_VOLUME};
		const SLboolean omreq[] = {SL_BOOLEAN_FALSE};
		rv = (*iengine)->CreateOutputMix(iengine, &outputmix, 1, omids, omreq);_print("CreateOutputMix rv=%d  outputmix=%x",rv,outputmix);
		if (rv!=0) return rv;
		rv = (*outputmix)->Realize(outputmix, SL_BOOLEAN_FALSE);_print("outputmix realize rv=%d",rv);
		if (rv!=0) return rv;

		// output mix source = output buffer that will go to speaker
		SLDataLocator_AndroidSimpleBufferQueue loc_bufq = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, outbuffers};
		SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 1,outsamplingrate*1000, outputbitspersample, outputbitspersample, SL_SPEAKER_FRONT_CENTER, SL_BYTEORDER_LITTLEENDIAN};
		SLDataSource audioSrc = {&loc_bufq, &format_pcm};

		// output mix sink = is the output mix object
		SLDataLocator_OutputMix loc_outmix = {SL_DATALOCATOR_OUTPUTMIX, outputmix};
		SLDataSink audioSnk = {&loc_outmix, NULL};

		// create the audio player with the source and sink above
		const SLInterfaceID apids[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
		const SLboolean apreq[] = {SL_BOOLEAN_TRUE};
		rv = (*iengine)->CreateAudioPlayer(iengine, &player, &audioSrc, &audioSnk, 1, apids, apreq);_print("CreateAudioPlayer rv=%d",rv);
		if (rv!=0) return rv;

		rv = (*player)->Realize(player, SL_BOOLEAN_FALSE);_print("player realize rv=%d",rv);
		if (rv!=0) return rv;

		rv = (*player)->GetInterface(player, SL_IID_PLAY,&iplayer);_print("player get interface rv=%d",rv);
		if (rv!=0) return rv;
		rv = (*player)->GetInterface(player, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &outbufferqueue);_print("player get buffer interface rv=%d",rv);

		// setup callback
		rv = (*outbufferqueue)->RegisterCallback(outbufferqueue,outcallbackstatic, this);_print("player register callback rv=%d",rv);
		if (rv!=0) return rv;

		prepareoutbuffers();

		int i;
		libroot::general::smartbuf tmp;
		tmp.resize(_maxoutbuffersize);
		memset(tmp.buffer,0,tmp.len);
		for (i=0;i<_totaloutbuffers;i++)
		{
			pushout(tmp);
		}

		isinitialized_output=true;
	}



	SLresult opensles::initinput(void (*cbIn)(opensles *thisopensles, void *context),int inbuffers,long maxinbuffersize, void *incontext, long insamplingrate, long inputchannels, int inputbitspersample)
	{
		SLresult rv;
		rv=initengine();
		if (rv!=0) return rv;

		if (isinitialized_input)
		{
			stopinput();
			closein();
		}

		if (iengine==NULL) return -1;

		if ((cbIn==NULL)||(inbuffers==0)||(maxinbuffersize==0)) return -1;

		if ((insamplingrate==0)||(inputchannels==0)) return -1;

		if ((inputbitspersample % 8) >0) return -1;

		inputcallback=cbIn;

		_totalinbuffers=inbuffers;
		_userincontext=incontext;
		_maxinbuffersize=maxinbuffersize;
		currentinbuffer=0;
		irecorder=0;
		// output mix
		const SLInterfaceID omids[] = {SL_IID_VOLUME};


		// input source = mic
		SLDataLocator_IODevice loc_mic={SL_DATALOCATOR_IODEVICE, SL_IODEVICE_AUDIOINPUT, SL_DEFAULTDEVICEID_AUDIOINPUT, NULL};
		SLDataSource audioSrc = {&loc_mic, NULL};

		SLDataLocator_AndroidSimpleBufferQueue loc_buf = {SL_DATALOCATOR_ANDROIDSIMPLEBUFFERQUEUE, inbuffers};
		SLDataFormat_PCM format_pcm = {SL_DATAFORMAT_PCM, 1,insamplingrate*1000, inputbitspersample, inputbitspersample, SL_SPEAKER_FRONT_CENTER, SL_BYTEORDER_LITTLEENDIAN};

		SLDataSink audioSnk = {&loc_buf, &format_pcm};

		const SLInterfaceID arid[] = {SL_IID_ANDROIDSIMPLEBUFFERQUEUE};
		const SLboolean arreq[] = {SL_BOOLEAN_TRUE};
		rv = (*iengine)->CreateAudioRecorder(iengine, &recorder, &audioSrc, &audioSnk, 1, arid, arreq);_print("\nCreateAudioRecorder rv=%d",rv);
		if (rv!=0) return rv;

		rv = (*recorder)->Realize(recorder, SL_BOOLEAN_FALSE);_print("Realize rv=%d",rv);
		if (rv!=0) return rv;

		rv = (*recorder)->GetInterface(recorder, SL_IID_RECORD, &irecorder);_print("\nGetInterface irecorder rv=%d",rv);
		if (rv!=0) return rv;

		rv = (*recorder)->GetInterface(recorder, SL_IID_ANDROIDSIMPLEBUFFERQUEUE, &inbufferqueue);_print("\nGetInterface inbufferqueue rv=%d",rv);
		if (rv!=0) return rv;

		rv = (*inbufferqueue)->RegisterCallback(inbufferqueue, incallbackstatic, this);_print("\RegisterCallback  rv=%d",rv);
		if (rv!=0) return rv;

		prepareinbuffers();

		int i;
		libroot::general::smartbuf tmp;
		tmp.resize(_maxinbuffersize);
		memset(tmp.buffer,0,tmp.len);
		for (i=0;i<_totalinbuffers;i++)
		{
			grabin(tmp);
		}

		isinitialized_input=true;
	}

	void opensles::prepareinbuffers()
	{
		int i;
		libroot::general::smartbuf tmp;
		tmp.resize(_maxinbuffersize);
		memset(tmp.buffer,0,tmp.len);

		for (i=0;i<_totalinbuffers;i++)
		{
			_inputbuffers.PushAtEnd(tmp.buffer, _maxinbuffersize,0);
		}
		tmp.freebuf();
	}

	void opensles::prepareoutbuffers()
	{
		int i;
		libroot::general::smartbuf tmp;
		tmp.resize(_maxoutbuffersize);
		memset(tmp.buffer,0,tmp.len);

		for (i=0;i<_totaloutbuffers;i++)
		{
			_outputbuffers.PushAtEnd(tmp.buffer, _maxoutbuffersize,0);
		}
		tmp.freebuf();
	}

	void opensles::pushout(void *data, long size)
	{
		_outputbuffers.SetDataAtItem_nosizechange(data,size,currentoutbuffer);
		(*outbufferqueue)->Enqueue(outbufferqueue, (char *)_outputbuffers.databuf.buffer+ _outputbuffers[currentoutbuffer]->offset, _outputbuffers[currentoutbuffer]->userdata1);
		currentoutbuffer++;
		if (currentoutbuffer>=_totaloutbuffers) currentoutbuffer=0;
	}
	void opensles::pushout(libroot::general::smartbuf &buf)
	{
		pushout(buf.buffer, buf.len);
	}

	void opensles::grabin(libroot::general::smartbuf &buf)
	{
		buf.setbuffer((char *) _inputbuffers.databuf.buffer+ _inputbuffers[currentinbuffer]->offset, _inputbuffers[currentinbuffer]->size);
		(*inbufferqueue)->Enqueue(inbufferqueue, (char *)_inputbuffers.databuf.buffer+ _inputbuffers[currentinbuffer]->offset, _inputbuffers[currentinbuffer]->size);

		currentinbuffer++;
		if (currentinbuffer>=_totalinbuffers) currentinbuffer=0;
	}

	void opensles::outcallbackstatic(SLAndroidSimpleBufferQueueItf buf, void *context)
	{
		if (context==NULL) return;
		opensles *me=(opensles *)context;
		me->outcallback(buf,context);
	}

	void opensles::outcallback(SLAndroidSimpleBufferQueueItf buf, void *context)
	{
		opensles *me=(opensles *)context;
		if (me->outputcallback!=NULL) me->outputcallback(me, me->_useroutcontext);
	}

	void opensles::incallbackstatic(SLAndroidSimpleBufferQueueItf buf, void *context)
	{
		if (context==NULL) return;
		opensles *me=(opensles *)context;
		me->incallback(buf,context);
	}
	void opensles::incallback(SLAndroidSimpleBufferQueueItf buf, void *context)
	{
		opensles *me=(opensles *)context;
		if (me->inputcallback!=NULL) me->inputcallback(me, me->_userincontext);
	}
	SLresult opensles::startoutput()
	{
		SLresult rv;
		if (iplayer==NULL) return -1;
		rv = (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_PLAYING);_print("player set play state rv=%d",rv);
		if (rv!=0) return rv;
		return 0;
	}
	SLresult opensles::pauseoutput()
	{
		SLresult rv;
		rv = (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_PAUSED);_print("player set play state rv=%d",rv);
		if (rv!=0) return rv;
		return 0;
	}
	SLresult opensles::stopoutput()
	{
		SLresult rv;
		rv = (*iplayer)->SetPlayState(iplayer, SL_PLAYSTATE_STOPPED);_print("player set play state rv=%d",rv);
		if (rv!=0) return rv;
		return 0;
	}

	void opensles::opensldestroy(SLObjectItf obj)
	{
		if (obj!=NULL) (*obj)->Destroy(obj);
	}
	void opensles::closeout()
	{
		if (player!=NULL)
		opensldestroy(player);
		if (outputmix!=NULL)
		opensldestroy(outputmix);

		player=NULL;iplayer=NULL;outbufferqueue=NULL;
		outputmix=NULL;
		isinitialized_output=false;
	}

	void opensles::closein()
	{
		if (recorder!=NULL)
		opensldestroy(recorder);

		recorder=NULL;irecorder=NULL;inbufferqueue=NULL;
		isinitialized_input=false;
	}

	void opensles::close()
	{
		closeout();
		closein();
		opensldestroy(engine);
		engine=NULL;iengine=NULL;
		isinitialized_engine=false;
	}
	SLresult opensles::startinput()
	{
		SLresult rv;
		if (irecorder==NULL) return -1;
		rv = (*irecorder)->SetRecordState(irecorder, SL_RECORDSTATE_RECORDING);
		if (rv!=0) return rv;
		return 0;
	}
	SLresult opensles::pauseinput()
	{
		SLresult rv;
		if (irecorder==NULL) return -1;
		rv = (*irecorder)->SetRecordState(irecorder, SL_RECORDSTATE_PAUSED);
		if (rv!=0) return rv;
		return 0;
	}
	SLresult opensles::stopinput()
	{
		SLresult rv;
		if (irecorder==NULL) return -1;
		rv = (*irecorder)->SetRecordState(irecorder, SL_RECORDSTATE_STOPPED);
		if (rv!=0) return rv;
		return 0;
	}
ns1end
#endif
