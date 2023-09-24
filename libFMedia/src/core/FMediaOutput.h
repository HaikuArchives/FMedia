#ifndef F_MEDIA_OUTPUT_H
#define F_MEDIA_OUTPUT_H

#include "FMediaDefs.h"
#include <BufferGroup.h>
#include <MediaNode.h>
#include <BufferProducer.h>
#include <stdio.h>
#include <string.h>

#include "FBufferManager.h"

#define F_ALLOW_DOWNSTREAM_BUFFERS		1
#define F_CREATE_BUFFERS_ON_CONNECT		2
#define F_RECLAIM_BUFFERS_ON_DISCONNECT 4
#define F_UPCAST_BUFFER_GROUP_CHANGES	8
//#define F_PASS_DOWNSTREAM_BUFFERS		0xf3c0001

#define F_SIMPLE_BUFFERS ( F_ALLOW_DOWNSTREAM_BUFFERS | F_CREATE_BUFFERS_ON_CONNECT | F_RECLAIM_BUFFERS_ON_DISCONNECT )

class FMediaInput;
class FMediaNode;

class FMediaOutput {
	public:
		FMediaOutput( FMediaNode *node, const char *name, const media_format& preferred, const media_format& required, int32 flags = 0, FBufferManager *bufferManager = NULL );
		FMediaOutput( FMediaNode *node, const char *name, media_type type = B_MEDIA_UNKNOWN_TYPE, int32 flags = 0, FBufferManager *bufferManager = NULL );
		void Setup( FMediaNode *node, const char *name, media_type type = B_MEDIA_UNKNOWN_TYPE, int32 flags = 0, FBufferManager *bufferManager = NULL );
		~FMediaOutput();
		
		// internal/for FMediaNode
		void Init( const media_node& node, const port_id& port, const int32 id = 0 );
		bool Is( const media_source& source ) { return( source == mOut.source && source.id == mOut.source.id ); }
		bool Is( const int32 source ) { return( source == mOut.source.id ); }
		status_t FormatProposal( media_format *format );
		status_t SetBufferGroup( BBufferGroup *group );		

		// called _after_ the connection has been made/broken, even after In/Output(Dis)Connected() of the node has been called
		status_t Connect( const media_destination& consumer, const media_format& format, char *io_name );
		void Disconnect( const media_destination& consumer );

		void SetFormat( const media_format& new_format );

		// Informational
		const char *Name() { return (const char *)&(mOut.name); }
		void SetName( const char *name ) { strncpy( mOut.name, name, B_MEDIA_NAME_LENGTH ); }
		media_format& Format() { return mOut.format; }
		bool IsConnected() const { return mConnected; }
		bool IsEnabled() const { return mEnabled; }

		media_output& GetOutput() { return mOut; }
		media_source& GetSource() { return mOut.source; }
		media_destination& GetDestination() { return mOut.destination; }
		
		void Enable( bool enabled = true ) { mEnabled = enabled; }

		// a MediaOutput's Latency is the time a buffer needs to wander down that stream.
		bigtime_t Latency() { return mLatency; }
		void SetLatency( bigtime_t latency ) { mLatency=latency; }
		
		FBufferManager *GetBufferManager() { return mBufferManager; }
		BBufferGroup *GetBufferGroup() { return( mBufferManager ? mBufferManager->Group() : NULL ); }

		virtual void AdditionalBufferRequested( const media_source &source, media_buffer_id prev_buffer, bigtime_t prev_time, const media_seek_tag *prev_tag );

		// Couple In/Outputs that handle the same buffers
		// and changes in BufferGroup and AdditionalBufferRequests will be upcasted
		void Couple( FMediaInput *input );
		FMediaInput *GetCoupledInput() { return mCoupledInput; }
	public:
		media_output mOut;
		
		bool mEnabled;
		bool mConnected;
		bigtime_t mLatency; // (downstream, not own node's processing latency)
		media_producer_status mDataStatus;
		
		int mFlags;
		FBufferManager *mBufferManager;

		FMediaNode *mNode;		
		media_format mPreferredFormat, mRequiredFormat;

		FMediaInput *mCoupledInput;
	protected:
		void mResetFormat();
};

#endif
