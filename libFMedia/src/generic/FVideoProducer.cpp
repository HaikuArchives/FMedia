#include "FVideoProducer.h"

#include <stdio.h>
#include <Buffer.h>
#include <BufferGroup.h>
#include <TimeSource.h>

#include "FMediaFormats.h"
#include "FViewBufferManager.h"

#define CALL //printf
#define INFO //printf
#define ERROR printf
#define WARNING printf
#define LATENCY //printf
#define FORMAT //printf
#define THREAD //printf
#define OFFLINE //printf


FVideoProducer::FVideoProducer( const media_format& requiredFormat, const media_format& preferredFormat, const char *name, FBufferManager *bufferMan, BMediaAddOn *addon = NULL )
	: BMediaNode( name ),
	  FControllable( name, B_MEDIA_NO_TYPE, B_MEDIA_RAW_VIDEO, addon ),
	  FSimpleThread( name ) {
	mFPS = NULL;
	mOutput = 0;
	mRequiredFormat = requiredFormat;
	mPreferredFormat = preferredFormat;
	mBufferManager = bufferMan;
	if( !mBufferManager ) mBufferManager = new FViewBufferManager( Name() );
	mBufferRequested = false;
	mSmartConsumer = false;
	mChangeFormat = mChangingFormat = false;
}

FVideoProducer::~FVideoProducer() {
	FSimpleThread::Stop();
}

void FVideoProducer::Setup() {
	mOutput = new FMediaOutput( this, "VideoOut", mPreferredFormat, mRequiredFormat, F_SIMPLE_BUFFERS, mBufferManager );
	AddOutput( mOutput );

	AddParameter( new FParameterGroup("FVideoProducer") );
	
	if( mRequiredFormat.u.raw_video.field_rate == 0 ) {
		float fps = (float)mPreferredFormat.u.raw_video.field_rate;
		if( fps == 0 ) fps = 15;
		mFPS = new FContinuousParameter( B_MEDIA_RAW_VIDEO, "Framerate", B_GENERIC, "fps", .5, 60.0, .1, fps );
		AddParameter( mFPS );
	}

	if( mRequiredFormat.u.raw_video.display.line_width == 0 ) {
		float w = (float)mPreferredFormat.u.raw_video.display.line_width;
		if( w == 0 ) w = 320;
		mWidth = new FContinuousParameter( B_MEDIA_RAW_VIDEO, "Width", B_GENERIC, "px", 10, 640, 1, w );
		AddParameter( mWidth );
	}

	if( mRequiredFormat.u.raw_video.display.line_count == 0 ) {
		float h = (float)mPreferredFormat.u.raw_video.display.line_count;
		if( h == 0 ) h = 320;
		mHeight = new FContinuousParameter( B_MEDIA_RAW_VIDEO, "Height", B_GENERIC, "px", 10, 480, 1, h );
		AddParameter( mHeight );
	}

	FControllable::Setup();
}

void FVideoProducer::OutputConnected( FMediaOutput *output, bool connected ) {
	if( connected ) {
		FSimpleThread::Start();
	//	ProducerFormatChanged( output->Format().u.raw_video );
		ChangeProducerFormat( output->Format() );
	} else FSimpleThread::Stop();
}


//--------------------------------------------------------------------------
//------------------- thread functions -------------------------------------
void FVideoProducer::ThreadFunction() {
	status_t err = B_OK;
	
	nextTime = TimeSource()->Now() + mTotalLatency;
	int skip = 0;

	bigtime_t Frequency = 1000000/mOutput->Format().u.raw_video.field_rate;
		
	while( !stStop && err == B_OK ) {
		if( !mRunning ) {
			snooze(Frequency);
		} else {
			if( mChangeFormat ) {
				mChangingFormat = true;
				FMediaNode::ChangeFormat( mOutput, mNewFormat );
				mChangeFormat = false;
				mChangingFormat = false;
			}
		
			Frequency = 1000000/mOutput->Format().u.raw_video.field_rate;
			nextTime += Frequency;
//			nextTime += mTotalLatency;
			bigtime_t now = TimeSource()->Now();
	
			skip=0;
			
			run_mode runmode = RunMode();
			if( runmode == B_RECORDING || runmode == B_OFFLINE ) {
				int stupid = 0;
				while( !stStop && !mBufferRequested ) {
					stupid++;
					if( stupid > 100 && !mSmartConsumer ) {
						// stupid consumer, doesnt Request AdditionalBuffers
						WARNING("Consumer might not be offline-capable, did not request additional buffers.\n");
						mBufferRequested = true;
					}
					snooze( 10000 );
				}
				mBufferRequested = false;
			} else {
				while( !stStop && nextTime < now ) {
					// frame-skipping
					printf("s"); fflush( stdout );
					nextTime += Frequency;
					skip++;
	//				printf("skip!"); fflush( stdout );
				}
		
				int i=0;
				while( !stStop && now < nextTime-mTotalLatency ) {
					i++;
	//				printf("Producer: %i to go\n", (nextTime-mTotalLatency)-now);
					snooze( 5000 ); //(int)((nextTime-mTotalLatency) - now) );
					now = TimeSource()->Now();
				}
			}

			bigtime_t start, end;
			end = 0;
			
			if( !mChangeFormat ) {
				INFO("Filling Buffer...\n");
				size_t sz = mOutput->Format().u.raw_video.display.line_count * mOutput->Format().u.raw_video.display.bytes_per_row;
				BBuffer *buffer = mOutput->GetBufferManager()->GetBuffer( sz );

				start = TimeSource()->RealTime();
				if( buffer ) {
					bigtime_t t = nextTime; //TimeSource()->PerformanceTimeFor( TimeSource()->RealTime() + mInternalLatency );
					f_filter_status status = MakeBuffer( buffer, t, skip );
					if( status >= F_DONE ) {
						if( status == B_OK ) {
							end = TimeSource()->RealTime();
						}
						buffer->Header()->start_time = t;
						buffer->Header()->size_used = sz;
						status_t err;
						if( (err = SendOrRecycleBuffer( buffer, mOutput ) ) == B_OK ) {
						//	INFO("Buffer sent\n");
						}
					} else
						RecycleBuffer( buffer );
				}
			}
					
			if( end && runmode == B_INCREASE_LATENCY ) {
				bigtime_t measuredLatency = (end-start);
				bigtime_t storedLatency = mInternalLatency;
			//	LATENCY("%s Latency: %iµs.\n", Name(), measuredLatency );
				if( measuredLatency > storedLatency + mSlowerLatencyTolerance || measuredLatency < storedLatency - mFasterLatencyTolerance ) {
					LATENCY( "%s: Latency change exceeds Jitter Tolerance: stored %i, measured %i usecs.\n", Name(), (int32)storedLatency, (int32)measuredLatency );
					mInternalLatency = measuredLatency;
					LATENCY("mInternal: %i\n", mInternalLatency );
//					PublishLatency();
					LATENCY("mTotal: %i, mInternal: %i\n", mTotalLatency, mInternalLatency );
		
					mSlowerLatencyTolerance = measuredLatency * F_SLOWER_LATENCY_TOLERANCE_FACTOR; 
					mFasterLatencyTolerance = measuredLatency * F_FASTER_LATENCY_TOLERANCE_FACTOR; 
					if( mSlowerLatencyTolerance < F_MIN_LATENCY_TOLERANCE ) mSlowerLatencyTolerance = F_MIN_LATENCY_TOLERANCE;
					if( mFasterLatencyTolerance < F_MIN_LATENCY_TOLERANCE ) mFasterLatencyTolerance = F_MIN_LATENCY_TOLERANCE;
				}
			}
		}

		//if( i > 1 ) printf("Had to wait %i cycles\n", i );
	}
}


f_filter_status FVideoProducer::MakeBuffer( BBuffer *buffer, bigtime_t performance_time, int skip_frames ) {
	return F_NOTHING_DONE;
}

void FVideoProducer::AdditionalBufferRequested( const media_source &source, media_buffer_id prev_buffer, bigtime_t prev_time, const media_seek_tag *prev_tag ) {
	OFFLINE("FVideoProducer::AdditionalBufferRequested( prev_time %i )\n", (int)prev_time );
	mBufferRequested = true;
	mSmartConsumer = true;
	
	FControllable::AdditionalBufferRequested( source, prev_buffer, prev_time, prev_tag );
}

void FVideoProducer::SetRunMode( run_mode mode ) {
	printf("FVideoProducer::SetRunMode( ");
	switch( mode ) {
			case B_OFFLINE:
				printf("Offline");
				nextTime = 0;
				break;
			case B_RECORDING:
				printf("Recording");
				break;
			case B_DECREASE_PRECISION:
				printf("Decrease Precision");
				nextTime = TimeSource()->Now();
				break;
			case B_INCREASE_LATENCY:
				printf("Increase Latency");
				nextTime = TimeSource()->Now();
				break;
			case B_DROP_DATA:
				printf("Drop Data");
				nextTime = TimeSource()->Now();
				break;
	}
	printf(" )\n");
	FControllable::SetRunMode( mode );
}

void FVideoProducer::ChangeProducerFormat( const media_format & new_format ) {
	mChangeFormat = false;
	while( mChangingFormat ) {
		printf("-"); fflush( stdout );
		snooze( 5000 );
	}

	mNewFormat = new_format;

	if( mFPS ) mNewFormat.u.raw_video.field_rate = mFPS->Get();
	if( mWidth ) mNewFormat.u.raw_video.display.line_width = mWidth->Get();
	if( mHeight ) mNewFormat.u.raw_video.display.line_count = mHeight->Get();
	mNewFormat.u.raw_video.display.bytes_per_row = mNewFormat.u.raw_video.display.line_width * FBytesPerPixel( mNewFormat.u.raw_video.display.format );

	mChangeFormat = true;
}

void FVideoProducer::ParameterChanged( FParameter *parameter ) {
	if( parameter == mFPS
		|| parameter == mWidth
		|| parameter == mHeight ) {
		INFO("%s: Changing Format due to parameter change\n", Name());
			ChangeProducerFormat( mOutput->Format() );
	} else {
		FControllable::ParameterChanged( parameter );
	}
}
