#include "FMediaNode.h"

#include <Buffer.h>
#include <TimeSource.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <BufferGroup.h>
#include <StopWatch.h>
#include <MediaRoster.h>

#include "FIterator.h"

#define CALL //printf
#define EVENT //printf
#define BUFFER //printf
#define UNHANDLED_EVENT //printf
#define INFO //printf
#define FORMAT //printf
#define DEBUG //printf
#define WARNING printf
#define ERROR printf
#define REQUEST_COMPLETED //printf
#define LATENCY //printf
#define TIMING //printf
#define OFFLINE //printf

#define ADDON_NAME "FMediaNode"

FMediaNode::FMediaNode( const char *name, media_type in_type, media_type out_type, BMediaAddOn *addon )
	:	BMediaNode( name ),
		BBufferConsumer( in_type ),
		BBufferProducer( out_type ),
		BMediaEventLooper(),
		mAddOn( addon ),
		mRunning( false ),
		mTotalLatency( F_INITIAL_LATENCY ),
		mSlowerLatencyTolerance( F_INITIAL_LATENCY_TOLERANCE ),
		mFasterLatencyTolerance( F_INITIAL_LATENCY_TOLERANCE ) {
	CALL("FMediaNode::ctor\n");

//	SetPriority( B_DISPLAY_PRIORITY );
	
	mInternalLatency = 0;
}

FMediaNode::~FMediaNode() {
	CALL("FMediaNode::dtor\n");

	BMediaEventLooper::Quit();

	// 2do: deletion of in/outputs
}


///////////////////////////////////////////////
// BMediaNode

BMediaAddOn* FMediaNode::AddOn( int32 *cookie ) const {
	CALL("FMediaNode::AddOn\n");
	return( mAddOn );
}

///////////////////////////////////////////////
// BBufferConsumer

status_t FMediaNode::AcceptFormat( const media_destination &dest, media_format *format ) {
	CALL("FMediaNode::AcceptFormat\n");

	if( FMediaInput *input = FindInput( dest ) ) {
		return( input->AcceptFormat( format ) );
	}
	
	return B_MEDIA_BAD_DESTINATION;
}

status_t FMediaNode::GetNextInput( int32 *cookie, media_input *input ) {
	CALL("FMediaNode::GetNextInput( %i )\n", *cookie );

	if( !*cookie ) {
		FIterator<FMediaInput*> *Iterator = new FIterator<FMediaInput*>( &mInputs );
		*cookie = (int32)Iterator;
	}

	FIterator<FMediaInput*> *Iterator = (FIterator<FMediaInput*> *)*cookie;
	if( Iterator->HasMore() ) {
		*input = (*Iterator->GetNext())->GetInput();
		return B_OK;
	}
	
	return B_BAD_INDEX;
}

void FMediaNode::DisposeInputCookie( int32 cookie ) {
	CALL("FMediaNode::DisposeInputCookie\n");
	FIterator<FMediaInput*> *Iterator = (FIterator<FMediaInput*> *)cookie;
	delete Iterator;
}

void FMediaNode::BufferReceived( BBuffer *buffer ) { 
	CALL("FMediaNode::BufferReceived( %p )\n", buffer );
	if( RunMode() == B_OFFLINE ) {
		int32 destinationID = buffer->Header()->destination;
		FMediaInput *input = FindInput( destinationID );
		// offline time is the smallest of the latest buffers on all inputs,
		// store.
		input->SetOfflineTime( buffer->Header()->start_time );
		SetOfflineTime();
	}

	status_t err;
	media_timed_event event( buffer->Header()->start_time,
							BTimedEventQueue::B_HANDLE_BUFFER,
							buffer, BTimedEventQueue::B_RECYCLE_BUFFER );
	err = EventQueue()->AddEvent( event );
	
	if( err ) buffer->Recycle();
}

void FMediaNode::SetRunMode( run_mode mode ) {
	OFFLINE("FMediaNode::SetRunMode( ");
	switch( mode ) {
			case B_OFFLINE:
				OFFLINE("Offline");
				break;
			case B_RECORDING:
				OFFLINE("Recording");
				break;
			case B_DECREASE_PRECISION:
				OFFLINE("Decrease Precision");
				break;
			case B_INCREASE_LATENCY:
				OFFLINE("Increase Latency");
				break;
			case B_DROP_DATA:
				OFFLINE("Drop Data");
				break;
	}
	OFFLINE(" )\n");
	BMediaEventLooper::SetRunMode( mode );
}

void FMediaNode::ProducerDataStatus( const media_destination& destination, int32 status, bigtime_t at_performance_time ) {
	OFFLINE("FMediaNode::ProducerDataStatus( %i )\n", status);

	media_timed_event event( at_performance_time, BTimedEventQueue::B_DATA_STATUS, (media_destination*)&destination, BTimedEventQueue::B_NO_CLEANUP, (int32)status, 0, NULL );
	EventQueue()->AddEvent( event );
}

void FMediaNode::AdditionalBufferRequested( const media_source &source, media_buffer_id prev_buffer, bigtime_t prev_time, const media_seek_tag *prev_tag ) {
	OFFLINE("%s: AdditionalBufferRequested( prev_time %i )\n", Name(), (int)prev_time );
	FMediaOutput *output = FindOutput( source );
	if( output ) {
		output->AdditionalBufferRequested( source, prev_buffer, prev_time, prev_tag );
		OFFLINE("AdditionalBufferRequest shoudl have been upcasted\n");
	} else {
		WARNING("---- Output not found in AdditionalBufferRequest\n");
	}
}

void FMediaNode::SetOfflineTime() {
	// queries offline time from inputs and propagates to BMediaEventLooper
	if( RunMode() == B_OFFLINE ) {
		bigtime_t t = -1;
	
		FIterator<FMediaInput*> inputs( &mInputs );
		while( inputs.HasMore() ) {
			FMediaInput *input = *inputs.GetNext();
			if( input && input->IsActive() ) { //IsActive returns false when producer data is unavailable.
				t = ( t==-1 ? input->OfflineTime() : MIN( t, input->OfflineTime() ) );
			}
		}

		if( t != -1 ) {
			OFFLINE("%s: Figured offline time from inputs: %i\n", Name(), t );
			BMediaEventLooper::SetOfflineTime( t );
		} else {
			BMediaEventLooper::SetOfflineTime( 0 );
		}
	}	
}

status_t FMediaNode::GetLatencyFor( const media_destination& destination, bigtime_t *latency, media_node_id *timesource ) {
	CALL("FMediaNode::GetLatencyFor\n");

	*timesource = ((BMediaNode*)TimeSource())->ID();

	if( FMediaInput *input = FindInput( destination ) ) {
		*latency = input->Latency();
		return B_OK;
	}

	*latency = mTotalLatency;
	ERROR("%s: Bad media destination in GetLatencyFor\n", Name());
	return B_MEDIA_BAD_DESTINATION;
}

status_t FMediaNode::Connected( const media_source& producer, const media_destination& destination, const media_format& format, media_input *_input ) {
	CALL("FMediaNode::Connected\n");

	if( FMediaInput *input = FindInput( destination ) ) {
		status_t err = input->Connected( producer, format, _input );
		if( err == B_OK ) InputConnected( input );
		return err;
	}

	return B_MEDIA_BAD_DESTINATION;
}

void FMediaNode::Disconnected( const media_source& producer, const media_destination& destination ) {
	CALL("FMediaNode::Disconnected\n");

	if( FMediaInput *input = FindInput( destination ) ) {
		InputConnected( input, false );
		input->Disconnected( producer );
		return;
	}
}

status_t FMediaNode::FormatChanged( const media_source& source, const media_destination& destination, int32 change_tag, const media_format& format ) {
	FORMAT("FMediaNode::FormatChanged(source,destination,change_tag,format)\n");
	
	FMediaInput *input = FindInput( destination );
	if( input ) {
		input->SetFormat( format );
		return( FormatChanged( input ) );
	} else {
		FMediaOutput *output = FindOutput( source );
		if( output ) {
			output->SetFormat( format );
			return( FormatChanged( output ) );
		}
	}

	return B_ERROR;
}

status_t FMediaNode::FormatChanged( FMediaInput *input ) {
	FORMAT("FMediaNode::FormatChanged(input)\n");
	return B_OK;
}

status_t FMediaNode::FormatChanged( FMediaOutput *output ) {
	FORMAT("FMediaNode::FormatChanged(output)\n");
	return B_OK;
}

void FMediaNode::ChangeFormat( FMediaOutput *output, const media_format &new_format ) {
	if( output ) {
		if( output->IsConnected() ) {
			bool wasEnabled = false;
			if( output->IsEnabled() ) {
				wasEnabled = true;
				output->Enable( false );
			}
			media_format format = new_format;
			status_t err = BBufferProducer::ChangeFormat( output->GetSource(), output->GetDestination(), &format );
			FORMAT("ChangeFormat: %s\n", strerror(err) );
			if( err == B_MEDIA_CHANGE_IN_PROGRESS ) {
				WARNING("%s::ChangeFormat: Change in Progress\n", Name() );
			} else if( err != B_OK ) {
				WARNING("%s::ChangeFormat: %s\n", Name(), strerror( err ) );
			} else {
				WARNING("%s::ChangeFormat: %s\n", Name(), strerror( err ) );
				output->SetFormat( new_format );
			}
			if( wasEnabled ) output->Enable( true );
		} else {
			output->SetFormat( new_format );
		}
//		mBufferManager.CreateBuffers( mOutput->Format(), 5 );
	}
}

status_t FMediaNode::HandleMessage( int32 code, const void *data, size_t sz ) {
	CALL("FMediaNode::HandleMessage\n");

	if( BBufferConsumer::HandleMessage( code, data, sz )
		&& BBufferProducer::HandleMessage( code, data, sz ) 
		&& BMediaEventLooper::HandleMessage( code, data, sz )
		&& BMediaNode::HandleMessage( code, data, sz ) )
		HandleBadMessage( code, data, sz );
		
	return B_ERROR;
}


///////////////////////////////////////////////
// BBufferProducer

status_t FMediaNode::FormatSuggestionRequested( media_type type, int32 quality, media_format *format ) {
	CALL("FMediaNode::FormatSuggestionRequested()\n");

	FIterator<FMediaOutput*> outputs( &mOutputs );

	while( outputs.HasMore() ) {
		FMediaOutput *output = *outputs.GetNext();
		if( output && output->Format().type == type ) {
			*format = output->Format(); // should be preferredFormat?
			return B_OK;
		}
	}
	
	return B_MEDIA_BAD_FORMAT;
}

status_t FMediaNode::FormatProposal( const media_source& source, media_format *format ) {
	FORMAT("FMediaNode::FormatProposal()\n");
	FMediaOutput *output = FindOutput( source );
	if( output ) {
		return output->FormatProposal( format );
	}

	FORMAT( ADDON_NAME"::FormatProposal - source not found\n");
	return B_MEDIA_BAD_SOURCE;
}

status_t FMediaNode::FormatChangeRequested( const media_source& source, const media_destination& destination, media_format* io_format, int32 *_deprecated_ ) {
	FORMAT("FMediaNode::FormatChangeRequested()\n");

	FMediaInput *input = FindInput( destination );
	if( input ) {
		status_t err = input->AcceptFormat( io_format );
/*		if( err == B_OK ) {
			if( input->IsConnected ) {
			}
		}
*/
		return err;
	}

	return B_MEDIA_BAD_FORMAT;
}

status_t FMediaNode::GetNextOutput( int32* cookie, media_output* output ) {
	CALL("FMediaNode::GetNextOutput( %i ).  count %i\n", *cookie, mOutputs.CountItems() );
	if( !*cookie ) {
		FIterator<FMediaOutput*> *Iterator = new FIterator<FMediaOutput*>( &mOutputs );
		*cookie = (int32)Iterator;
	}

	FIterator<FMediaOutput*> *Iterator = (FIterator<FMediaOutput*> *)*cookie;
	if( Iterator->HasMore() ) {
		*output = (*Iterator->GetNext())->GetOutput();
		return B_OK;
	}

	return B_BAD_INDEX;
}

status_t FMediaNode::DisposeOutputCookie( int32 cookie ) {
	CALL("FMediaNode::DisposeOutputCookie\n");
	FIterator<FMediaOutput*> *Iterator = (FIterator<FMediaOutput*> *)cookie;
	delete Iterator;
	
	return B_OK;
}


status_t FMediaNode::SetBufferGroup( const media_source& source, BBufferGroup *group ) {
	CALL("FMediaNode::SetBufferGroup( %p )\n", group);

	if( FMediaOutput *output = FindOutput( source ) ) {
		return output->SetBufferGroup( group );
	}

	return B_MEDIA_BAD_SOURCE;
}

status_t FMediaNode::PrepareToConnect( const media_source& source, const media_destination& destination, media_format *format, media_source *out_source, char *out_name ) {
	CALL("FMediaNode::PrepareToConnect()\n");

	if( FMediaOutput *output = FindOutput( source ) ) {
		status_t ret = output->FormatProposal( format );
		if( ret == B_OK ) {
			*out_source = source;
			::strncpy( out_name, output->Name(), B_MEDIA_NAME_LENGTH );
		}
		return ret;
	}

	return B_MEDIA_BAD_SOURCE;
}

void FMediaNode::Connect( status_t error, const media_source& source, const media_destination& destination, const media_format& format, char *io_name ) {
	CALL("FMediaNode::Connect()\n");

	if( FMediaOutput *output = FindOutput( source ) ) {
		status_t err = output->Connect( destination, format, io_name );

		if( err == B_OK ) OutputConnected( output, true );

		media_node_id timesource_id;
		bigtime_t downstream_latency;
		err = FindLatencyFor(output->GetOutput().destination, &downstream_latency, &timesource_id);
		if( err != B_OK ) return;
		output->SetLatency( downstream_latency );

		return;
	}

	return;
}

void FMediaNode::Disconnect( const media_source &source, const media_destination& destination ) {
	CALL("FMediaNode::Disconnect()\n");

	if( FMediaOutput *output = FindOutput( source ) ) {
		OutputConnected( output, false );
		output->Disconnect( destination );
		return;
	}
}

void FMediaNode::LateNoticeReceived( const media_source& what, bigtime_t how_much, bigtime_t performance_time ) {
	LATENCY("-------- FMediaNode::LateNoticeReceived()\n");
}

void FMediaNode::EnableOutput( const media_source& source, bool enabled, int32* _deprecated_ ) {
	CALL("FMediaNode::EnableOutput()\n");
	
	if( FMediaOutput *output = FindOutput( source ) ) {
		output->Enable( enabled );
		return;
	}
}

status_t FMediaNode::GetLatency(bigtime_t *out_latency) {
	CALL("FMediaNode::GetLatency\n");

	bigtime_t scheduling_latency=2000, myLatency = 0;
	status_t err = B_OK;
	
	// Downstream Latencies
	mDownstreamLatency = 0;

	FIterator<FMediaOutput*> outputs( &mOutputs );
	while( outputs.HasMore() ) {
		FMediaOutput *output = *outputs.GetNext();
		if( output ) {
			LATENCY("%s.%s Latency: %i usecs.\n", Name(), output->Name(), output->Latency() );
			mDownstreamLatency += output->Latency();
			myLatency += output->Latency();
		}
	}

	// Processing Latencies
	myLatency += mInternalLatency;

	mTotalLatency = myLatency;
	*out_latency = mTotalLatency;

/*	status_t err;
	err = BBufferProducer::GetLatency( out_latency );
	if( err != B_OK ) ERROR("FMediaNode::GetLatency inherited error: %s\n", strerror(err) );
	*out_latency += 20000; //mLatency;
*/

	LATENCY( ADDON_NAME": GetLatency(): %iµs\n", (uint32)(*out_latency) );
	
	return( B_OK );
}

void FMediaNode::LatencyChanged( const media_source &source, const media_destination &destination, bigtime_t newLatency, uint32 flags ) {
	CALL("FMediaNode::LatencyChanged\n");
	FMediaOutput *output = FindOutput( source );
	if( output ) {
		output->SetLatency( newLatency );
	}

	PublishLatency();
}

void FMediaNode::PublishLatency() {
	// publish upwards.
	bigtime_t mLatency;
	GetLatency( &mLatency );
	
	SetEventLatency( mLatency+SchedulingLatency() );

	FIterator<FMediaInput*> inputs( &mInputs );
	while( inputs.HasMore() ) {
		FMediaInput *input = *inputs.GetNext();
		if( input ) {
			SendLatencyChange( input->GetSource(), input->GetDestination(), mLatency + SchedulingLatency() );
		}
	}
}


///////////////////////////////////////////////
// BMediaNode

status_t FMediaNode::RequestCompleted(const media_request_info &info) {
	REQUEST_COMPLETED("%s: Request Completed: ", Name());
	switch( info.what ) {
		case media_request_info::B_SET_VIDEO_CLIPPING_FOR:
			REQUEST_COMPLETED("SetVideoClippingFor()");
			break;
		case media_request_info::B_REQUEST_FORMAT_CHANGE:
			REQUEST_COMPLETED("RequestFormatChange()");
			break;
		case media_request_info::B_SET_OUTPUT_ENABLED:
			REQUEST_COMPLETED("SetOutputEnabled()");
			break;
		case media_request_info::B_SET_OUTPUT_BUFFERS_FOR:
			REQUEST_COMPLETED("SetOutputBuffersFor()");
			break;
		case media_request_info::B_FORMAT_CHANGED:
			REQUEST_COMPLETED("FormatChanged()");
			break;
	}
	REQUEST_COMPLETED("\n");
	REQUEST_COMPLETED("	change-tag: %i\n", info.change_tag );
	REQUEST_COMPLETED("	status: %s (%8.8x, %4.4s)\n", strerror( info.status ), info.status, &(info.status));
}

///////////////////////////////////////////////
// BMediaEventLooper

void FMediaNode::NodeRegistered() {
	CALL("FMediaNode::NodeRegistered\n");

	Setup();
/*
	if( FIterator<FMediaInput*> *inputs = mInputs.CreateIterator() ) {
		while( inputs->HasMoreValues() ) {
			FMediaInput *input = *inputs->GetNextValue();
			if( input ) {
				input->Init( Node(), ControlPort(), (int32)input );
			}
		}
		delete inputs;
	}

	if( FIterator<FMediaOutput*> *outputs = mOutputs.CreateIterator() ) {
		while( outputs->HasMoreValues() ) {
			FMediaOutput *output = *outputs->GetNextValue();
			if( output ) {
				output->Init( Node(), ControlPort(), (int32)output );
			}
		}
		delete outputs;
	}
*/
	Run();
}

void FMediaNode::HandleEvent( const media_timed_event *event, bigtime_t lateness, bool realTimeEvent ) {
	EVENT("FMediaNode::HandleEvent\n");
	
	bigtime_t now = TimeSource()?TimeSource()->Now():0;
	bigtime_t etime = event->event_time;
	TIMING("[Timing Info] eventTime-now: %i, lateness: %i\n", now-etime, lateness );

	switch( event->type ) {
		case BTimedEventQueue::B_HANDLE_BUFFER:
			{
				BBuffer *buffer = const_cast<BBuffer*>((BBuffer*)event->pointer);
				if( buffer ) {
					TIMING("[Timing Info] bufferTime-eventTime: %i\n", (buffer->Header()?buffer->Header()->start_time - event->event_time:-1) );
					HandleBufferWrap( buffer, lateness );
				}
			}
			break;
			
		case BTimedEventQueue::B_START:
			EVENT("[ %s ] FMediaNode::HandleEvent( B_START )\n", Name() );
			mRunning = true;
			break;
			
		case BTimedEventQueue::B_STOP:
			EVENT("[ %s ] FMediaNode::HandleEvent( B_STOP )\n", Name() );
			mRunning = false;
			EventQueue()->FlushEvents( 0, BTimedEventQueue::B_ALWAYS, true, BTimedEventQueue::B_HANDLE_BUFFER );
			break;
			
		case BTimedEventQueue::B_DATA_STATUS:
			{
				EVENT("FMediaNode::HandleEvent( %s, B_DATA_STATUS, %i)\n", Name(), (media_producer_status)event->data );
				media_destination *dest = (media_destination*)event->pointer;
				FMediaInput *input = FindInput( *dest );
				if( input ) {
					input->SetDataStatus( (media_producer_status)event->data );
				}
			}
			break;
			
		default:
			UNHANDLED_EVENT("Unhandled Event in FMediaNode (%i)\n", event->type );
			break;
	}
}

void FMediaNode::HandleBufferWrap( BBuffer *buffer, bigtime_t lateness ) {
	int32 destinationID = buffer->Header()->destination;

	FMediaInput *input = FindInput( destinationID );
//	DEBUG("[ %s ] FMediaNode::HandleBufferWrap( input %p, %s running\n", Name(), input, mRunning?"indeed":"not" );
	if( input && mRunning ) {
		bigtime_t start = TimeSource()->RealTime();
			if( !HandleBuffer( buffer, input ) ) return;
		bigtime_t end = TimeSource()->RealTime();
	
		bigtime_t measuredLatency = (end-start)+500;
		bigtime_t storedLatency = input->Latency();
	//	LATENCY("%s.%s Latency: %i usecs.\n", Name(), MediaInputs[destination]->Name(), measuredLatency );
		if( measuredLatency > storedLatency + mSlowerLatencyTolerance || measuredLatency < storedLatency - mFasterLatencyTolerance ) {
			LATENCY( "%s: Latency change exceeds Jitter Tolerance: stored %i, measured %i usecs.\n", Name(), (int32)storedLatency, (int32)measuredLatency );
			input->SetLatency( measuredLatency );
			PublishLatency();

			mSlowerLatencyTolerance = measuredLatency * F_SLOWER_LATENCY_TOLERANCE_FACTOR; 
			mFasterLatencyTolerance = measuredLatency * F_FASTER_LATENCY_TOLERANCE_FACTOR; 
			if( mSlowerLatencyTolerance < F_MIN_LATENCY_TOLERANCE ) mSlowerLatencyTolerance = F_MIN_LATENCY_TOLERANCE;
			if( mFasterLatencyTolerance < F_MIN_LATENCY_TOLERANCE ) mFasterLatencyTolerance = F_MIN_LATENCY_TOLERANCE;
		}
	} else {
		BUFFER("FMediaNode::HandleBufferWrap: Unhandled Destination (%i) or nor running. Recycling.\n", destinationID );
		RecycleBuffer( buffer, input );
	}
}

void FMediaNode::InputConnected( FMediaInput *i, bool connected ) {
	// generic implementation does nothing.
}

void FMediaNode::OutputConnected( FMediaOutput *o, bool connected ) {
	// generic implementation does nothing.
}

status_t FMediaNode::SendOrRecycleBuffer( BBuffer *buffer, FMediaOutput *output, FMediaInput *input ) {
	status_t err = B_OK;
	if( !buffer ) return B_ERROR;
	if( !output ) {
		DEBUG("%s: Buffer not sent. (no output)\n", Name() );
		RecycleBuffer( buffer, input );
		return B_ERROR;
	}
	if( !input ) input = output->GetCoupledInput();
	if( !output->IsConnected() || !output->IsEnabled() ) {
		DEBUG("%s: Buffer not sent. (not %s)\n", Name(), output->IsConnected()?"enabled":"connected" );
		RecycleBuffer( buffer, input );
		return B_ERROR;
	}
	if( (err=SendBuffer(buffer, output->GetDestination())) != B_OK ) {
		DEBUG("output: %s, sport %i, sid %i, dport %i, did %i\n", output->Name(), output->GetSource().port, output->GetSource().id, output->GetDestination().port, output->GetDestination().id );
		DEBUG("%s.%s: Buffer not sent. (%s)\n", Name(), output->Name(), err==B_OK?"not connected":strerror( err ) );
		RecycleBuffer( buffer, input );
		return( F_BUFFER_RECYCLED );
	}
	return( B_OK );
}

status_t FMediaNode::RecycleBuffer( BBuffer *buffer, FMediaInput *input ) {
	status_t err = B_OK;
	if( !buffer ) return B_ERROR;

	run_mode mode = RunMode();
	if( input && mode == B_OFFLINE || mode == B_RECORDING ) {
		status_t err = RequestAdditionalBuffer( input->GetSource(), buffer );
		OFFLINE("%s: Requesting Additional Buffer: %s\n", Name(), strerror( err ) );
	}
	
	buffer->Recycle();
	INFO("%s: Buffer recycled\n", Name() );

	OFFLINE("%s: Not requesting another buffer... whyever (input %p)\n", Name(), input);
	
	return( err );
}

void FMediaNode::UpcastBufferGroup( FMediaInput *input, BBufferGroup *group ) {
	if( input ) {
		if( !group || group->InitCheck() != B_OK ) {
			ERROR( ADDON_NAME": Couldn't initialize ViewBufferGroup.\n");
		} else {
			int32 change_tag = 1;	
			status_t err = BBufferConsumer::SetOutputBuffersFor( input->GetSource(), input->GetDestination(), group, NULL, &change_tag, true);
			if( err ) {
				ERROR( ADDON_NAME": Couldn't broadcast ViewBufferGroup for upstream: %s\n", strerror( err ) );
			} else {
				INFO(  ADDON_NAME": broadcasted ViewBufferGroup for upstream.\n");
			}
		}	
	}
}

FMediaOutput *FMediaNode::FindOutput( const media_source& source ) {
	FIterator<FMediaOutput*> outputs( &mOutputs );
	FMediaOutput *output;
	while( outputs.HasMore() && NULL != (output = *outputs.GetNext()) ) {
		if( output && output->Is( source ) ) return( output );
	}
	return NULL;
}

FMediaInput *FMediaNode::FindInput( const media_destination& destination ) {
	FIterator<FMediaInput*> inputs( &mInputs );
	FMediaInput *input;
	while( inputs.HasMore() && NULL != (input = *inputs.GetNext() ) ) {
		if( input && input->Is( destination ) ) return( input );
	}
	return NULL;
}

FMediaOutput *FMediaNode::FindOutput( const int32 source ) {
	FIterator<FMediaOutput*> outputs( &mOutputs );
	FMediaOutput * output;
	while( outputs.HasMore() && NULL != (output = *outputs.GetNext() ) ) {
		if( output && output->Is( source ) ) return( output );
	}
	return NULL;
}

FMediaInput *FMediaNode::FindInput( const int32 destination ) {
	FIterator<FMediaInput*> inputs( &mInputs );
	FMediaInput *input;
	while( inputs.HasMore() && NULL != (input = *inputs.GetNext()) ) {
		if( input && input->Is( destination ) ) return( input );
	}
	return NULL;
}

bool FMediaNode::HandleBuffer( BBuffer *inBuffer, FMediaInput *onInput ) {
	CALL("FMediaNode::HandleBuffer()\n");
	RecycleBuffer( inBuffer, onInput );
	return false;
}

void FMediaNode::Setup() {
	CALL("FMediaNode::Setup()\n");
	// generic does nothing
}
