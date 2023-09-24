#ifndef F_MEDIA_INPUT_H
#define F_MEDIA_INPUT_H

#include "FMediaDefs.h"
#include <BufferGroup.h>
#include <MediaNode.h>
#include "FBufferManager.h"
#include <string.h>

class FMediaOutput;
class FMediaNode;

class FMediaInput {
	public:
		FMediaInput( FMediaNode *node, const char *name, const media_format& preferred, const media_format& required );
		FMediaInput( FMediaNode *node, const char *name, media_type type = B_MEDIA_UNKNOWN_TYPE );
		void Setup( FMediaNode *node, const char *name, media_type type = B_MEDIA_UNKNOWN_TYPE );
		~FMediaInput();
		
		// internal/for FMediaNode
		void Init( const media_node& node, const port_id& port, const int32 id = 0 );
		bool Is( const media_destination& destination ) { return( destination == mIn.destination ); }
		bool Is( const int32 destination ) { return( destination == mIn.destination.id ); }
		status_t AcceptFormat( media_format *format );
		
		// called _after_ the connection has been made/broken, even after In/Output(Dis)Connected() of the node has been called
		status_t Connected( const media_source& producer, const media_format& format, media_input *input );
		void Disconnected( const media_source& producer );
		
		// a MediaInputs Latency is the time the node needs to process a buffer from that input
		bigtime_t Latency() { return mLatency; }
		void SetLatency( bigtime_t latency ) { mLatency=latency; }

		void SetFormat( const media_format& new_format ) { mIn.format = new_format; }

		// Informational
		const char *Name() { return (const char *)&(mIn.name); }
		void SetName( const char *name ) { strncpy( mIn.name, name, B_MEDIA_NAME_LENGTH ); }
		const media_format& Format() { return mIn.format; }
		media_input& GetInput() { return mIn; }
		media_source& GetSource() { return mIn.source; }
		media_destination& GetDestination() { return mIn.destination; }
		
		void SetDataStatus( const media_producer_status& status ) { mDataStatus = status; }
		
		bool IsConnected() { return mConnected; }
		bool IsActive() { return mDataStatus == B_DATA_AVAILABLE; }
		
		bigtime_t OfflineTime() { return mOfflineTime; }
		void SetOfflineTime( bigtime_t t ) { mOfflineTime = t; }
		
		void Couple( FMediaOutput *output );
				
	protected:
		media_input mIn;
		
		bigtime_t mLatency;
		media_producer_status mDataStatus;
		
		media_format mPreferredFormat, mRequiredFormat;
		bool mConnected;
		
		bigtime_t mOfflineTime;

		FMediaNode *mNode;
		FMediaOutput *mCoupledOutput;
	protected:
};

#endif
