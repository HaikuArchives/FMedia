#include "FMediaOutput.h"
#include "FMediaInput.h"
#include "FMediaFormats.h"
#include "FMediaInput.h"
#include "FMediaNode.h"
#include <BufferConsumer.h>

#include <stdio.h>
#include <string.h>

#define ERROR printf
#define WARNING printf
#define FORMAT printf
#define METADATA printf
#define INFO //printf

FMediaOutput::FMediaOutput( FMediaNode *node, const char *name, const media_format& preferred, const media_format& required, int32 flags = 0, FBufferManager *bufferManager ) {
	Setup( node, name, required.type, flags, bufferManager );
	mPreferredFormat = preferred;
	mRequiredFormat = required;	
	mResetFormat();
}

FMediaOutput::FMediaOutput( FMediaNode *node, const char *name, media_type type, int32 flags, FBufferManager *bufferManager  ) {
	Setup( node, name, type, flags, bufferManager );
}

void FMediaOutput::Setup( FMediaNode *node, const char *name, media_type type, int32 flags, FBufferManager *bufferManager  ) {
	mNode = node;
	mEnabled = false;
	mConnected = false;
	mLatency = F_INITIAL_LATENCY;
	mDataStatus = (media_producer_status)B_OK;

	mFlags = flags;

	if( NULL == (mBufferManager = bufferManager ) ) {
		mBufferManager = new FBufferManager( name );
	}

	mPreferredFormat.type = type;
	mRequiredFormat.type = type;
	mResetFormat();

	::strncpy( mOut.name, name, B_MEDIA_NAME_LENGTH );
	
	mCoupledInput = NULL;
}

FMediaOutput::~FMediaOutput() {
	if( mBufferManager ) delete mBufferManager;
}

void FMediaOutput::Init( const media_node& node, const port_id& port, const int32 id ) {
	mOut.node = node;
	mOut.source.port = port;
	mOut.source.id = id;
	mOut.destination = media_destination::null;
//	printf("%s.Init(.., %i, %i)\n", Name(), port, id );

	Enable();
}

status_t FMediaOutput::Connect( const media_destination& consumer, const media_format& format, char *io_name ) {
	mConnected = true;
	mOut.format = format;
	mOut.destination = consumer;
	::strncpy( io_name, mOut.name, B_MEDIA_NAME_LENGTH );

//	printf("%s.Connect -> port %i, id %i\n", Name(), mOut.destination.port, mOut.destination.id );

	status_t err = B_OK;
	if( (mFlags & F_CREATE_BUFFERS_ON_CONNECT ) && mBufferManager ) {
		if( mBufferManager->Group() ) {
			INFO( "%p (%s) already has a BufferGroup, not creating.\n", mBufferManager, mBufferManager->Name() );
		} else {
			err = mBufferManager->CreateBuffers( (media_format&)format );
			if( err ) ERROR("%s.Connect: Couldn't create Buffers: %s\n", mOut.name, strerror(err));
		}
	}

	return( err );
}

void FMediaOutput::Disconnect( const media_destination& consumer ) {
	mConnected = false;
	mResetFormat();
	mOut.destination = media_destination::null;

	if( (mFlags & F_RECLAIM_BUFFERS_ON_DISCONNECT ) && mBufferManager ) {
		mBufferManager->ReclaimBuffers();
	}
}

void FMediaOutput::SetFormat( const media_format& new_format ) {
	size_t currentSize = FMediaIdealBufferSizeForFormat( &mOut.format );
	size_t newSize = FMediaIdealBufferSizeForFormat( &new_format );
	if( currentSize != newSize && mBufferManager ) {
		int32 n = mBufferManager->CountBuffers();
		if( n ) {
			FORMAT("%s: New format needs different buffer size, recreating Buffers\n", Name());
			mBufferManager->ReclaimBuffers();
//			FORMAT("Reclaimed\n");
			mBufferManager->CreateBuffers( new_format, n, newSize );
			FORMAT("%s: New Buffers created (%i buffers of size %i)\n", Name(), n, newSize );
		}
	}
	mOut.format = new_format;
}

status_t FMediaOutput::FormatProposal( media_format *format ) {
//	return( FAcceptAndSpecializeMediaFormat( format, &mPreferredFormat, &mRequiredFormat ) );
	if( format->type == B_MEDIA_ANY_TYPE || mRequiredFormat.type == B_MEDIA_ANY_TYPE ) return( B_OK );
	if( B_OK != (mRequiredFormat.Matches( format ) ) ) {
		FORMAT("Generic match function returns error\n");
		return( FAcceptAndSpecializeMediaFormat( format, &mPreferredFormat, &mRequiredFormat ) );
		//return( B_MEDIA_BAD_FORMAT );
	}
	format->SpecializeTo( &mRequiredFormat );
	return B_OK;
}

void FMediaOutput::mResetFormat() {
/*
	if( mRequiredFormat.type == B_MEDIA_RAW_VIDEO ) {
		mPreferredFormat.u.raw_video = F320x240x25RGB32VideoFormat;
		mRequiredFormat.u.raw_video = FStandardRGB32VideoFormat;
	}
*/
	mOut.format = mRequiredFormat;
}

status_t FMediaOutput::SetBufferGroup( BBufferGroup *group ) {
	//if( mFlags & F_PASS_DOWNSTREAM_BUFFERS ) return F_PASS_DOWNSTREAM_BUFFERS;
	if( !( mFlags & F_ALLOW_DOWNSTREAM_BUFFERS ) ) return B_ERROR;
	INFO("SetBufferGroup for output \"%s\"\n", mOut.name );
	status_t err =  mBufferManager->UseBuffers( mOut.format, group );

	/* F_UPCAST_BUFFER_GROUP_CHANGES...
	if( err == B_OK && mBufferChangeNode && mUpstreamInput ) {
		INFO("------\nUpcastBufferGroup\n");
		mBufferChangeNode->UpcastBufferGroup( mUpstreamInput, group );
	}
	*/
	return err;
}

void FMediaOutput::Couple( FMediaInput *input ) {
	if( mCoupledInput != input ) {
		mCoupledInput = input;
		input->Couple( this );
	}
}

void FMediaOutput::AdditionalBufferRequested( const media_source &source, media_buffer_id prev_buffer, bigtime_t prev_time, const media_seek_tag *prev_tag ) {
	if( mCoupledInput ) {
		mNode->RequestAdditionalBuffer( mCoupledInput->GetSource(), prev_buffer );
	}
//	 else
//		WARNING("%s.%s: Could not upcast AdditionalBufferRequest\n", mNode->Name(), Name() );	
}
