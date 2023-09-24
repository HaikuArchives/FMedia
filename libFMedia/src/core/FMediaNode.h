#ifndef F_MEDIA_FILTER_H
#define F_MEDIA_FILTER_H

#include "MediaNode.h"
#include "MediaEventLooper.h"
#include "BufferConsumer.h"
#include "BufferProducer.h"
#include "MediaAddOn.h"
#include "BufferGroup.h"
#include "Buffer.h"
#include "FMediaInput.h"
#include "FMediaOutput.h"
#include "FMediaDefs.h"

#include "FArray.h"
#include "FString.h"

//! a MediaNode, the F style. implements most things MediaKit.
/*!
	FMediaNode derives from the MediaKit's BBufferConsumer, BBufferProducer and BMediaEventLooper,
	implementing most of their functions.
	
	It handles in- and outputs of the node thru lists of FMediaInput/FMediaOutput.
	You can at any time add those (remove is unstable yet) by using AddInput() and AddOutput().
	
	Some degree of latency adaption is implemented for consumers
	(thru measuring the time spent in the core function, HandleBuffer() )
	with some care about the B_OFFLINE runmode.
*/
class FMediaNode
	: public BBufferConsumer, public BBufferProducer, public BMediaEventLooper {

	public:
		FMediaNode( const char *name = "untitled FMediaNode", media_type in_type = B_MEDIA_UNKNOWN_TYPE, media_type out_type = B_MEDIA_UNKNOWN_TYPE, BMediaAddOn *addon = NULL );
		~FMediaNode();

		virtual status_t HandleMessage( int32, const void *, size_t );
			
	protected:
		virtual void AdditionalBufferRequested( const media_source &source, media_buffer_id prev_buffer, bigtime_t prev_time, const media_seek_tag *prev_tag );
		virtual void SetRunMode( run_mode mode );

	private:				
			// BMediaNode
			virtual BMediaAddOn* AddOn( int32 *cookie ) const;
			virtual status_t RequestCompleted(const media_request_info &info);

		// Offline-mode stuff
			virtual void SetOfflineTime(); // queries offline time from inputs and propagates to BMediaEventLooper

		// BMediaEventLooper
			virtual void NodeRegistered();
			virtual void HandleEvent( const media_timed_event *event, bigtime_t lateness, bool realTimeEvent );

		// BBufferConsumer
			virtual status_t AcceptFormat( const media_destination &dest, media_format *format );
			virtual status_t GetNextInput( int32 *cookie, media_input *input );
			virtual void DisposeInputCookie( int32 cookie );
			virtual void BufferReceived( BBuffer *buffer );
			virtual void ProducerDataStatus( const media_destination& destination, int32 status, bigtime_t at_performance_time );
			virtual status_t GetLatencyFor( const media_destination& destination, bigtime_t *latency, media_node_id *timesource );
			virtual status_t Connected( const media_source& producer, const media_destination& destination, const media_format& format, media_input *input );
			virtual void Disconnected( const media_source& producer, const media_destination& destination );
			virtual status_t FormatChanged( const media_source& producer, const media_destination& consumer, int32 change_tag, const media_format& format );

		// BBufferProducer
			virtual status_t FormatSuggestionRequested( media_type type, int32 quality, media_format *format );
			virtual status_t FormatProposal( const media_source& output, media_format *format );
			virtual status_t FormatChangeRequested( const media_source& source, const media_destination& destination, media_format* io_format, int32 *_deprecated_ );
			virtual status_t GetNextOutput( int32* cookie, media_output* out_output );
			virtual status_t DisposeOutputCookie( int32 cookie );
			virtual status_t SetBufferGroup( const media_source& for_source, BBufferGroup *group );
			virtual status_t PrepareToConnect( const media_source& what, const media_destination& where, media_format *format, media_source *out_source, char *out_name );
			virtual void Connect( status_t error, const media_source& source, const media_destination& destination, const media_format& format, char *io_name );
			virtual void Disconnect( const media_source &what, const media_destination& where );
			virtual void LateNoticeReceived( const media_source& what, bigtime_t how_much, bigtime_t performance_time );
			virtual void EnableOutput( const media_source& what, bool enabled, int32* _deprecated_ );
			virtual status_t GetLatency(bigtime_t *out_latency);
			virtual void LatencyChanged( const media_source &source, const media_destination &destination, bigtime_t newLatency, uint32 flags );
			status_t FormatProposal( media_format *format );
			status_t SetBufferGroup( BBufferGroup *group );		

		// Own internal functions
			// latency
			virtual void PublishLatency();
		
			// HandleBufferWrap measures latency, calls HandleBuffer
			void HandleBufferWrap( BBuffer *inBuffer, bigtime_t lateness );

	protected:
		// FMediaNode API
		// hooks
			
//! hook: Set all up
/*
	Overriding this function is the way to add inputs, outputs and, in case of FControllable, parameters and
	do any other setup work that is necessary. Always call your direct parent's Setup().
	
	Called after the node was registered with the media_server (on BMediaNode's NodeRegistered() ).
*/
			virtual void Setup();
			
//! hook: called when we received a new buffer to handle
/*!
	\param inBuffer the BBuffer to process
	\param input the FMediaInput on which the buffer was received
	\return true if you did the job (latency will be counted), false if some error occurred.
	
	be sure you SendOrRecycle() or just Recycle() the buffer, or call my HandleBuffer(), which does just that.
*/
			virtual bool HandleBuffer( BBuffer *inBuffer, FMediaInput *input );

//! hook to respond on connection/disconnection of one of our inputs.
/*!
	called after the connection has been made/broken.
*/
			virtual void InputConnected( FMediaInput *input, bool connected = true );
//! hook to respond on connection/disconnection of one of our outputs.
/*!
	called after the connection has been made/broken.
*/
			virtual void OutputConnected( FMediaOutput *output, bool connected = true );
			
//! hook to respond to format changes.
/*!
	\param input the FMediaInput the format change occurred on.

	called when the format has actually been changed, too late to snort now about the specifics of the format.
*/
			virtual status_t FormatChanged( FMediaInput *input );
			virtual status_t FormatChanged( FMediaOutput *output );
			
//! Change the format of some output, notifying connected nodes of this event.
			void ChangeFormat( FMediaOutput *output, const media_format &new_format );
			
//! tries to pass buffer to output, recycling it (with input) when not successful
/*!
	\param buffer the BBuffer in question
	\param output the FMediaOutput to try to send the buffer to.
	\param input the FMediaInput used when the buffer is recycled (see RecycleBuffer() ).
*/
			status_t SendOrRecycleBuffer( BBuffer *buffer, FMediaOutput *output, FMediaInput *input = NULL );

//! recycles the buffer, requesting a new one from input when in offline mode.
/*!
	\param buffer the BBuffer in question
	\param input the FMediaInput to request an additional buffer from when in offline mode.
*/
			status_t RecycleBuffer( BBuffer *buffer, FMediaInput *input = NULL );

//! asks the node connected to input to use the BufferGroup group.
			void UpcastBufferGroup( FMediaInput *input, BBufferGroup *group );

		// functionality
			
//! Add out to the list of Outputs, initializing it when init is true.
			void AddOutput( FMediaOutput *out, bool init=true ) {
					mOutputs.AddItem( out );
					if( init ) out->Init(Node(), ControlPort(), (int32)out);
				}
//! Add in to the list of Inputs, initializing it when init is true.
			void AddInput( FMediaInput *in, bool init=true ) {
					mInputs.AddItem( in );
					if( init ) in->Init(Node(), ControlPort(), (int32)in);
				}

			int32 RemoveOutput( FMediaOutput *out ) { mOutputs.RemoveItem( out ); delete out; }
			int32 RemoveInput( FMediaInput *in ) { mInputs.RemoveItem( in ); delete in; }
			
			FMediaOutput *FindOutput( const media_source& source );
			FMediaInput *FindInput( const media_destination& destination );

			FMediaOutput *FindOutput( const int32 sourceID );
			FMediaInput *FindInput( const int32 destinationID );

	protected:
		BMediaAddOn* mAddOn;
		
		bool mRunning;
		bigtime_t mTotalLatency;
		bigtime_t mDownstreamLatency, mInternalLatency;
		bigtime_t mSlowerLatencyTolerance, mFasterLatencyTolerance;
		
		FArray<FMediaInput*> mInputs;
		FArray<FMediaOutput*> mOutputs;
		
		friend class FMediaInput;
		friend class FMediaOutput;
};

#endif
