#include "FFilter.h"
#include <stdio.h>
#include <string.h>
#include "FMediaFormats.h"
#include "FPixel.h"

#define CALL //printf
#define INFO //printf
#define ERROR printf
#define WARNING printf
#define FORMAT printf

FFilter::FFilter( const media_format& requiredFormat, const media_format& preferredFormat, const char *name, BMediaAddOn *addon = NULL )
	: BMediaNode( name ),
	  FControlDataNode( name, requiredFormat.type, requiredFormat.type, addon ),
	  mRequiredFormat( requiredFormat ),
	  mPreferredFormat( preferredFormat ) {
	  mInput = NULL;
}

FFilter::~FFilter() {
}

void FFilter::Setup() {
	mInput = new FMediaInput( this, "Input", mPreferredFormat, mRequiredFormat );
	AddInput( mInput );
	mOutput = NULL;
	FControllable::Setup();
}

bool FFilter::HandleBuffer( BBuffer *inBuffer, FMediaInput *onInput  ) {
	INFO("FFilter::HandleBuffer\n");
	if( onInput == mInput ) { //&& mOutput) {
		// security checks
		if( !inBuffer && inBuffer->SizeUsed() < FMediaIdealBufferSizeForFormat( &mInput->Format() ) ) {
			WARNING("%s: Buffer size too small for format. Not processing\n", Name() );
			return false;
		}
		
		return Filter( inBuffer );
	} else {
		return inherited::HandleBuffer( inBuffer, onInput );
	}
}

void FFilter::InputConnected( FMediaInput *input, bool connected ) {
	if( connected && input == mInput ) {
		// set output format
		mOutput = new FMediaOutput( this, "Output", mInput->Format(), mInput->Format() );
		AddOutput( mOutput, true );
		mOutput->Couple( mInput );
		FilterFormatChanged( mInput->Format() );
	} else if( !connected && input == mInput ) {
		if( mOutput ) RemoveOutput( mOutput );
		mOutput = NULL;
	}
}

status_t FFilter::FormatChanged( FMediaInput *input ) {
	FORMAT("FFilter::FormatChanged\n");
	if( input == mInput ) {
		if( mOutput )
			ChangeFormat( mOutput, input->Format() );
	 	FilterFormatChanged( input->Format() );
		return B_OK;
	}
	return B_ERROR;
}

void FFilter::OutputConnected( FMediaOutput *output, bool connected ) {
}

void FFilter::FilterFormatChanged( const media_format& new_format ) {
	// generic is noop
}
