#include "FMediaInput.h"
#include "FMediaFormats.h"
#include "FMediaOutput.h"

#include <BufferConsumer.h>

#include <string.h>
#include <stdio.h>

#define ERROR printf
#define METADATA printf
#define CALL //printf


FMediaInput::FMediaInput( FMediaNode *node, const char *name, const media_format& preferred, const media_format& required ) {
	Setup( node, name, required.type );
	mPreferredFormat = preferred;
	mRequiredFormat = required;	
	mIn.format = required;
}
	
FMediaInput::FMediaInput( FMediaNode *node, const char *name, media_type type ) 
	: mLatency( F_INITIAL_LATENCY ) {
	Setup( node, name, type );
}

void FMediaInput::Setup( FMediaNode *node, const char *name, media_type type ) {
	mNode = node;
	mDataStatus = (media_producer_status)B_DATA_AVAILABLE;
	mConnected = false;
	mLatency = F_INITIAL_LATENCY;
	
	mPreferredFormat.type = type;
	mRequiredFormat.type = type;
	mIn.format = mRequiredFormat;

	::strncpy( mIn.name, name, B_MEDIA_NAME_LENGTH );
	
	mOfflineTime = -1;
	
	mCoupledOutput = NULL;
}

FMediaInput::~FMediaInput() {
}

void FMediaInput::Init( const media_node& node, const port_id& port, const int32 id ) {
	mIn.node = node;
	mIn.destination.port = port;
	mIn.destination.id = id;
	mIn.source = media_source::null;
}

status_t FMediaInput::Connected( const media_source& producer, const media_format& format, media_input *input ) {
	mConnected = true;
	mIn.format = format;
	mIn.source = producer;
	*input = mIn;

	return( B_OK );
}

void FMediaInput::Disconnected( const media_source& producer ) {
	mConnected = false;
	mIn.source = media_source::null;
}

status_t FMediaInput::AcceptFormat( media_format *format ) {
//	return( FAcceptAndSpecializeMediaFormat( format, &mPreferredFormat, &mRequiredFormat ) );
	CALL("FMediaInput::AcceptFormat\n");
	METADATA("%s: AcceptFormat() (MetaData %i, %p)\n", Name(), format->MetaDataSize(), format->MetaData() );
	if( format->type == B_MEDIA_ANY_TYPE || mRequiredFormat.type == B_MEDIA_ANY_TYPE ) return( B_OK );
	if( !(mRequiredFormat.Matches( format ) ) ) {
		return( FAcceptAndSpecializeMediaFormat( format, &mPreferredFormat, &mRequiredFormat ) );
//		return( B_MEDIA_BAD_FORMAT );
	}
	return( FAcceptAndSpecializeMediaFormat( format, &mPreferredFormat, &mRequiredFormat ) );
//	format->SpecializeTo( &mRequiredFormat );
	return B_OK;
}

void FMediaInput::Couple( FMediaOutput *output ) {
	if( mCoupledOutput != output ) {
		mCoupledOutput = output;
		output->Couple( this );
	}
}