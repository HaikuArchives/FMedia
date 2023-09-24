#include "FCrop.h"
#include <stdio.h>
#include <ParameterWeb.h>
#include "FMediaFormats.h"
#include "FPixel.h"

#define CALL //printf
#define INFO //printf
#define ERROR printf(ADDON_NAME": "); printf
#define FORMAT //printf

FCrop::FCrop( const char *name = ADDON_NAME, BMediaAddOn *addon = NULL )
	: FCopyFilter( FGetWildcardVideoFormat(), FGetWildcardVideoFormat(), name, addon ),
	  BMediaNode( name ) {
	  MakeParameterWeb();
	  mLeft = mTop = mWidth = mHeight = NULL;
	  oldW = oldH = -1;
}

FCrop::~FCrop() {
}

void FCrop::Setup() {
	mLeft = new FContinuousParameter( B_MEDIA_RAW_VIDEO, "Left", B_GENERIC, "%", 0.0, 100.0, .1, 0.0 );
	AddParameter( mLeft );
	mTop = new FContinuousParameter( B_MEDIA_RAW_VIDEO, "Top", B_GENERIC, "%", 0.0, 100.0, .1, 0.0);
	AddParameter( mTop );
	mWidth = new FContinuousParameter( B_MEDIA_RAW_VIDEO, "Width", B_GENERIC, "%", 0.0, 100.0, .1, 100.0 );
	AddParameter( mWidth );
	mHeight = new FContinuousParameter( B_MEDIA_RAW_VIDEO, "Height", B_GENERIC, "%", 0.0, 100.0, .1, 100.0 );
	AddParameter( mHeight );

	FCopyFilter::Setup();
}

bool FCrop::Filter( BBuffer *thruBuffer, BBuffer *outBuffer ) {
	FPixel *inData = (FPixel *) thruBuffer->Data();
	FPixel *outData = (FPixel *) outBuffer->Data();

	//-----------------------
	
	int w = mInput->Format().u.raw_video.display.line_width;
	int h = mInput->Format().u.raw_video.display.line_count;
	
	int Width = (mWidth->Get()/100) * w;
	int Height = (mHeight->Get()/100) * h;
	int Left = (mLeft->Get()/100) * w;
	int Top = (mTop->Get()/100) * h;
	if( Left + Width > w ) Left = w-Width;
	if( Top + Height > h ) Top = h-Height;

	for( int y=0; y<Height; y++ ) {
		memcpy( &outData[ (Width*y) ], &inData[ (w * (y+Top)) + Left ], Width*sizeof(FPixel) );
	}
	
	//-----------------------
	
	SendOrRecycleBuffer( thruBuffer, mThruput, mInput );
	SendOrRecycleBuffer( outBuffer, mOutput );

	return true;
} 

status_t FCrop::AdaptFormat( media_format *io_format ) {
	int h = io_format->u.raw_video.display.line_count;
	int w = io_format->u.raw_video.display.line_width;
	
	int bytes_per_pixel = (io_format->u.raw_video.display.bytes_per_row/w);

	h = (mHeight->Get()/100) * h;
	w = (mWidth->Get()/100) * w;

	io_format->u.raw_video.display.line_count = h;
	io_format->u.raw_video.display.line_width = w;
	io_format->u.raw_video.display.bytes_per_row = bytes_per_pixel*w;
	
	if( h != oldH || w != oldW ) {
		ChangeFormat( mOutput, *io_format );
	}
	oldH = h;
	oldW = w;
	return B_OK;
}

void FCrop::ParameterChanged( int32 id ) {
	media_format format = mInput->Format();
	AdaptFormat( &format );
}
