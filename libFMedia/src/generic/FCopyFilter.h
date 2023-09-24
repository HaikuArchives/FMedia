#ifndef F_FCopyFilter_H
#define F_FCopyFilter_H

#include "FControllable.h"
#include "FBufferManager.h"

//! a media filter that produces buffers in a different format than it takes them, also providing a thruput of the original data.
/*!
	derive from FCopyFilter whenever you want to produce a media node that takes one input stream of media,
	and produce a different one as output. FCopyFilter provides you with mInput and mOutput, and mThruput for the original signal.
	
	Probably the most important function (after Filter(), that is) is AdaptFormat(), where you modify the input format to
	produce the output one.
*/
class FCopyFilter : public FControllable {
	public:
		FCopyFilter( const media_format& preferredFormat, const media_format& requiredFormat, const char *name, BMediaAddOn *addon = NULL );
		virtual ~FCopyFilter();
				
	protected:
		// FMediaNode
		virtual void InputConnected( FMediaInput *input, bool connected = true );
		virtual bool HandleBuffer( BBuffer *inBuffer, FMediaInput *onInput );
		virtual status_t FormatChanged( FMediaInput *input );

	protected:

		virtual void Setup();
//! hook for your filter function
/*!
	\param thruBuffer contains the input signal, should be SendOrRecycle()'d to mThruput/mInput or at least Recycle()'d
	\param outBuffer an empty buffer for you to fill with the output signal. SizeAvailable() should be large enough for the format you specified with AdaptFormat(). SendOrRecycle() to mOutput, or Recycle(), without an input.
*/
		virtual bool Filter( BBuffer *thruBuffer, BBuffer *outBuffer ) = 0;

//! hook to produce output format from input format
/*!
	implement this, and change appropriate values in inoutFormat.
	if you return B_OK, it will be set to the Output and outBuffers sent to Filter() will be large enough for the new format.
*/
		virtual status_t AdaptFormat( media_format* inoutFormat );
		
	protected:
		media_format mRequiredFormat, mPreferredFormat, mOutputFormat;

		FMediaInput *mInput;
		FMediaOutput *mThruput, *mOutput;

	private:
		FBufferManager *mBufferManager;
};

#endif
