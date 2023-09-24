#include "FCopyJoint.h"
#include <stdio.h>
#include <string.h>
#include "FMediaFormats.h"
#include "FPixel.h"

#define CALL //printf
#define INFO //printf
#define ERROR printf("FCopyJoint: "); printf
#define WARNING printf("FCopyJoint: "); printf
#define FORMAT printf

FCopyJoint::FCopyJoint( const media_format& preferredFormat, const media_format& requiredFormat, const char *name, BMediaAddOn *addon = NULL )
	: BMediaNode( name ),
	  FControllable( name, requiredFormat.type, requiredFormat.type, addon ),
	  mRequiredFormat( requiredFormat ),
	  mPreferredFormat( preferredFormat ),
	  mOutputFormat( preferredFormat ) {

	mInputA = mInputB = NULL;
	mOutputA = mOutputB = mOutput = NULL;
	mHoldB = NULL;
}

FCopyJoint::~FCopyJoint() {
}

void FCopyJoint::Setup() {
	mInputA = new FMediaInput( this, "InputA", mPreferredFormat, mRequiredFormat );
	AddInput( mInputA );
	mInputB = new FMediaInput( this, "InputB", mPreferredFormat, mRequiredFormat );
	AddInput( mInputB );
	
	FControllable::Setup();
}

void FCopyJoint::InputConnected( FMediaInput *input, bool connected ) {

	if( connected && input == mInputA ) {
		media_format outFormat = mInputA->Format();

		mOutput = new FMediaOutput( this, "Output", outFormat, outFormat, F_SIMPLE_BUFFERS );
		AddOutput( mOutput );

		mOutputA = new FMediaOutput( this, "OutputA", outFormat, outFormat, F_SIMPLE_BUFFERS );
		AddOutput( mOutputA );

		mOutputA->Couple( mInputA );

	} else if( !connected && input == mInputA ) {
		RemoveOutput( mOutputA );
		mOutputA = NULL;
	} else if( connected && input == mInputB ) {
		media_format outFormat = mInputB->Format();

		mOutputB = new FMediaOutput( this, "OutputB", outFormat, outFormat, F_SIMPLE_BUFFERS );
		AddOutput( mOutputB );

		mOutputB->Couple( mInputB );

		RemoveOutput( mOutput );
		mOutput = NULL;
	} else if( !connected && input == mInputB ) {
		RemoveOutput( mOutputB );
		mOutputB = NULL;
	}

}

bool FCopyJoint::HandleBuffer( BBuffer *Buffer, FMediaInput *onInput ) {
	if( onInput == mInputB ) {
		if( mHoldB ) {
			SendOrRecycleBuffer( mHoldB, mOutputB, mInputB );
		}

		if( !Buffer && Buffer->SizeUsed() < FMediaIdealBufferSizeForFormat( &mInputB->Format() ) ) {
			WARNING("%s: Buffer size too small for format. Not processing\n", Name() );
			mHoldB = NULL;
			return false;
		}

		mHoldB = Buffer;
	} else if( onInput == mInputA ) {
		if( !Buffer && Buffer->SizeUsed() < FMediaIdealBufferSizeForFormat( &mInputA->Format() ) ) {
			WARNING("%s: Buffer size too small for format. Not processing\n", Name() );
			return false;
		}

		if( mHoldB ) {
			size_t sz = FMediaIdealBufferSizeForFormat( &mOutput->Format() );
			FBufferManager *bufferMan = mOutput->GetBufferManager();
			INFO("(FConverter (%s): Requesting buffer size %i from FBufferManager %p\n", Name(), sz, bufferMan );
			BBuffer *outBuffer = bufferMan?bufferMan->GetBuffer( sz ):NULL;
			if( !outBuffer ) {
				WARNING("Didn't get buffer\n");
				SendOrRecycleBuffer( Buffer, mOutputA, mInputA );
				return false;
			} else {
				INFO("%s: Got Buffer, sz %i\n", Name(), outBuffer->SizeAvailable());
			}
			
			// thruBuffer header -> outBuffer header,
			//! /todo adapt format
			media_header *outHeader = outBuffer->Header();
			media_header *thruHeader = Buffer->Header();
			outHeader->start_time = thruHeader->start_time;
			outHeader->user_data_type = thruHeader->user_data_type;
			memcpy( outHeader->user_data, thruHeader->user_data, 64 );	

			if( Filter( Buffer, mHoldB, outBuffer ) == F_DONE ) {
				SendOrRecycleBuffer( outBuffer, mOutput );
			} else {
				RecycleBuffer( outBuffer );
			}
			SendOrRecycleBuffer( Buffer, mOutputA, mInputA );
			
		} else {
			SendOrRecycleBuffer( Buffer, mOutputA, mInputA );
		}
	}
	
	//-----------------------
		
	return true;
}

status_t FCopyJoint::FormatChanged( FMediaInput *input ) {
	FORMAT("%s: Format Changed\n", Name());
	if( input == mInputA ) {
		if( mOutputA ) {
			media_format format = input->Format();
			ChangeFormat( mOutputA, format);
			FORMAT("%s: OutputA format changed\n", Name());
		}
		return B_OK;
	} else if( input == mInputB ) {
		if( mOutputB ) {
			if( mHoldB ) RecycleBuffer( mHoldB, mInputB );
			mHoldB = NULL;
			media_format format = input->Format();
			ChangeFormat( mOutputB, format);
			FORMAT("%s: OutputB format changed\n", Name());
		}
		return B_OK;
	}
	return B_ERROR;
}
