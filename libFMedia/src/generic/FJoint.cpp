#include "FJoint.h"
#include <stdio.h>
#include <string.h>
#include "FMediaFormats.h"
#include "FPixel.h"

#define CALL //printf
#define INFO //printf
#define ERROR printf("FJoint: "); printf
#define WARNING printf("FJoint: "); printf
#define FORMAT printf

FJoint::FJoint( const media_format& preferredFormat, const media_format& requiredFormat, const char *name, BMediaAddOn *addon = NULL )
	: BMediaNode( name ),
	  FControllable( name, requiredFormat.type, requiredFormat.type, addon ),
	  mRequiredFormat( requiredFormat ),
	  mPreferredFormat( preferredFormat ),
	  mOutputFormat( preferredFormat ) {

	mInputA = mInputB = NULL;
	mOutputA = mOutputB = NULL;
	mHoldA = NULL;
}

FJoint::~FJoint() {
}

void FJoint::Setup() {
	mInputA = new FMediaInput( this, "InputA", mPreferredFormat, mRequiredFormat );
	AddInput( mInputA );
	mInputB = new FMediaInput( this, "InputB", mPreferredFormat, mRequiredFormat );
	AddInput( mInputB );
	
	FControllable::Setup();
}

void FJoint::InputConnected( FMediaInput *input, bool connected ) {

	if( connected && input == mInputA ) {
		media_format outFormat = mInputA->Format();

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
	} else if( !connected && input == mInputB ) {
		RemoveOutput( mOutputB );
		mOutputB = NULL;
	}

}

bool FJoint::HandleBuffer( BBuffer *Buffer, FMediaInput *onInput ) {
	if( onInput == mInputB ) {
//		printf("B"); fflush(stdout);

		if( mHoldA ) {
			Filter( mHoldA, Buffer );
			mHoldA = NULL;
		} else {
			mHold.Add( Buffer );
		}
/*		if( mHoldB ) {
			SendOrRecycleBuffer( mHoldB, mOutputB, mInputB );
			mHoldB = NULL;
		}

		if( !Buffer && Buffer->SizeUsed() < FMediaIdealBufferSizeForFormat( &mInputB->Format() ) ) {
			WARNING("%s: Buffer size too small for format. Not processing\n", Name() );
			mHoldB = NULL;
			return false;
		}

		mHoldB = Buffer;
*/
	} else if( onInput == mInputA ) {
		//printf("A"); fflush(stdout);
		if( !Buffer && Buffer->SizeUsed() < FMediaIdealBufferSizeForFormat( &mInputA->Format() ) ) {
			WARNING("%s: Buffer size too small for format. Not processing\n", Name() );
			return false;
		}

		BBuffer *holdB = mHold.Get( Buffer->Header()->start_time - 500000 );
		if( holdB != NULL ) {
			Filter( Buffer, holdB );
//			SendOrRecycleBuffer( holdB, mOutputB, mInputB );
		} else {
			//SendOrRecycleBuffer( Buffer, mOutputA, mInputA );
			if( mHoldA ) {
				RecycleBuffer( mHoldA, mInputA );
			}
			mHoldA = Buffer;
		}
	}
	
	//-----------------------
		
	return true;
}

status_t FJoint::FormatChanged( FMediaInput *input ) {
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
//			if( mHoldA ) RecycleBuffer( mHoldB, mInputB );
//			mHoldB = NULL;
			media_format format = input->Format();
			ChangeFormat( mOutputB, format);
			FORMAT("%s: OutputB format changed\n", Name());
		}
		return B_OK;
	}
	return B_ERROR;
}
