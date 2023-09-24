#ifndef F_FVideoProducer_H
#define F_FVideoProducer_H

#include "FControllable.h"
#include "FParameter.h"
#include "FSimpleThread.h"

//! a generic video producer node
/*!
	set requested formats with the constructor, but initialize in ProducerFormatChanged().
	MakeBuffer() is the main function to implement.
*/
class FVideoProducer : public FControllable, public FSimpleThread {
	public:
		FVideoProducer( const media_format& requiredFormat, const media_format& preferredFormat, const char *name, FBufferManager *bufferMan = NULL, BMediaAddOn *addon = NULL );
		~FVideoProducer();

//! will add mOutput
		virtual void Setup();

	private:		
		// BBufferProducer
		virtual void AdditionalBufferRequested( const media_source &source, media_buffer_id prev_buffer, bigtime_t prev_time, const media_seek_tag *prev_tag );
		virtual void SetRunMode( run_mode mode );
		// FMediaNode
		virtual void OutputConnected( FMediaOutput *output, bool connected = true );

	protected:
//! Handles changes of Framerate, Width, Height and initiates a format change
		virtual void ParameterChanged( FParameter *parameter );

//! FMediaFilter::FormatChanged() -> ProducerFormatChanged() wrap
//		virtual status_t FormatChanged( FMediaInput *input );

		// FVideoProducer
//! called when connected or format has been changed on downstream request
		virtual void ProducerFormatChanged( const media_raw_video_format& new_format ) {
			; // mediakit FormatChanged handling nyi, but called when connected.
		}
//! propagate a change in production format to downstream nodes
		void ChangeProducerFormat( const media_format &new_format );

//! fill a buffer with data
/*!
	\param buffer the BBuffer to fill. Should be large enough to handle current format.
	\param performance_time the performance time (in my timesource's conception of time) when the buffer will "get analog".
	\param skip_frames how many frames have been skipped since the last call to MakeBuffer.
*/		
		virtual f_filter_status MakeBuffer( BBuffer *buffer, bigtime_t performance_time, int skip_frames = 0 );
		
	protected:
		FContinuousParameter *mFPS;
		FContinuousParameter *mWidth;
		FContinuousParameter *mHeight;
		// CSpace?

		FMediaOutput *mOutput;

		virtual void ThreadFunction();

		FBufferManager *mBufferManager;

		media_format mRequiredFormat, mPreferredFormat;

		bigtime_t nextTime;
		bool mBufferRequested;
		bool mSmartConsumer;
		
		bool mChangeFormat;
		bool mChangingFormat;
		media_format mNewFormat;
};

#endif
