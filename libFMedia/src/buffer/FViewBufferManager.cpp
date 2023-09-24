#include "FViewBufferManager.h"

#include <stdio.h>
#include <string.h>

#define CALL printf
#define INFO printf
#define ERROR printf

FViewBufferManager::FViewBufferManager( const char* name ) 
	: FBufferManager( name, 5 ) {
	CALL("FViewBufferManager::FViewBufferManager()\n");
	mBitmap = NULL;
	mView = NULL;
}

FViewBufferManager::~FViewBufferManager() {
	CALL("FViewBufferManager::~FViewBufferManager()\n");
	if( nBuffers ) {
		if( mBitmap ) {
			for( int i=0; i<nBuffers; i++ ) {
				if( mBitmap[i] ) delete mBitmap[i];
			}
			delete mBitmap;
		}
	}
}

status_t FViewBufferManager::CreateBuffers( const media_format& format, int32 n, size_t sz ) {
	CALL("FViewBufferManager::CreateBuffers()\n");

	status_t err = B_OK;

	if( n > aBuffers ) Reallocate( n+3 );
	nBuffers = n;
	mBitmap = new BBitmap*[nBuffers];
	mBuffer = new BBuffer*[nBuffers];
	for( int i=0; i<nBuffers; i++ ) {
		mBitmap[i]=NULL;
		mBuffer[i]=NULL;
	}
	mOwnBuffers = true;

	mBufferGroup = new BBufferGroup;
	err = mBufferGroup->InitCheck();
	if( err != B_OK  ) {
		ERROR("FViewBufferManager::CreateBuffers - BufferGroup creation: %s\n", strerror( err ) );
		return( err );
	}

	err = CreateBitmaps( &(format.u.raw_video) );
	if( err != B_OK )
		return( err );


	for( int i=0; i<nBuffers && !err; i++ ) { // create buffers
		buffer_clone_info bc_info; 

			if ((bc_info.area = area_for(mBitmap[i]->Bits())) == B_ERROR)
				ERROR("FViewBufferManager::CreateBuffers - didn't get area_for(Bitmap)\n");;
			bc_info.offset = 0;
			bc_info.size = (size_t)mBitmap[i]->BitsLength();
			bc_info.flags = i;
			bc_info.buffer = 0;

		mBuffer[i] = NULL; 
		status_t err = mBufferGroup->AddBuffer(bc_info, &mBuffer[i]); 	
		if( err!=B_OK ) return( err );
	}

	mBufferGroup->CountBuffers( &nBuffers );
	mBufferGroup->GetBufferList( nBuffers, mBuffer );
	for( int i=0; i<nBuffers; i++ ) {
		buffer_clone_info bc_info = mBuffer[i]->CloneInfo();
		INFO("Buffer %i: %p", i, mBuffer[i] );
		INFO(" - area %i, ofs %i, size %i, flags %i\n", bc_info.area, bc_info.offset, bc_info.size, bc_info.flags );

		RegisterMetaHeader( i );
	}
	INFO("%s View Buffers total: %i, size %i\n", Name(), nBuffers, mBitmap[0]->BitsLength() );	
	
	return B_OK;
}

status_t FViewBufferManager::UseBuffers( const media_format& format, BBufferGroup *group ) {
	CALL("FViewBufferManager::UseBuffers()\n");
	return B_ERROR;
	FBufferManager::UseBuffers( format, group );
 
	if( !mBitmap ) {
		media_raw_video_format vformat = format.u.raw_video;
		status_t err = CreateBitmaps( &vformat );
		if( err != B_OK )
			return( err );
	}

	return( B_OK );
}

status_t FViewBufferManager::CreateBitmaps( const media_raw_video_format* format, uint32 flags ) {
	CALL("FViewBufferManager::CreateBitmaps()\n");

	BRect bounds( 0, 0, format->display.line_width-1, format->display.line_count-1 );

	mBitmap = new BBitmap*[ nBuffers ];
	mView = new BView*[ nBuffers ];
	for( int i=0; i<nBuffers; i++ ) {
//		mBitmap[i] = new BBitmap( bounds, flags | B_BITMAP_IS_AREA, format->display.format, format->display.bytes_per_row );
		mBitmap[i] = new BBitmap( bounds, format->display.format, true, true );
		if( !mBitmap[i] || !mBitmap[i]->IsValid() ) {
			INFO("Bitmap[%i] (%p) is invalid\n", i, mBitmap[i] );
			return B_ERROR;
		}
		mView[i] = new BView( bounds, "Dummy", B_FOLLOW_ALL_SIDES, B_WILL_DRAW );
		mBitmap[i]->AddChild( mView[i] );
	}
	
	return( B_OK );
}


void FViewBufferManager::Copy( BBuffer *Buffer, BBitmap *Bitmap ) {
	if( Buffer->SizeUsed() != Bitmap->BitsLength() ) {
		INFO("BitmapBuffer::Copy Buffer->Bitmap: Different Sizes: Buffer %i, Bitmap %i\n", Buffer->SizeUsed(), Bitmap->BitsLength() );
	}
	INFO("BitmapBuffer::Copy Buffer->Bitmap: Buffer %p -> Bitmap %i\n", Buffer, Bitmap );
	
	size_t size = min_c( Buffer->SizeUsed(), Bitmap->BitsLength() );
	INFO("Copy Buffer->Bitmap, %i bytes\n", size );
	memcpy( Bitmap->Bits(), Buffer->Data(), size );
}

void FViewBufferManager::Copy( BBitmap *Bitmap, BBuffer *Buffer ) {
	if( Buffer->SizeUsed() != Bitmap->BitsLength() ) {
		INFO("BitmapBuffer::Copy Bitmap->Buffer: Different Sizes: Bitmap %i, Buffer %i\n", Bitmap->BitsLength(), Buffer->SizeUsed() );
	}
	INFO("BitmapBuffer::Copy Bitmap->Buffer: Bitmap %p -> Buffer %i\n", Bitmap, Buffer );

	size_t size = min_c( Buffer->SizeUsed(), Bitmap->BitsLength() );
	INFO("Copy Bitmap->Buffer, %i bytes\n", size );
	memcpy( Buffer->Data(), Bitmap->Bits(), size );
}


void FViewBufferManager::RegisterMetaHeader( int i ) {
	mBuffer[i]->Header()->user_data_type = F_META_HEADER_TYPE;
	FMetaHeader *mHeader = (FMetaHeader*)(&(mBuffer[i]->Header()->user_data));
	mHeader->Bitmap = mBitmap[i];
	mHeader->View = mView[i];
}
