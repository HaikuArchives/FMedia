#include "FBufferManager.h"
#include "FMediaFormats.h"

#include <string.h>
#include <stdio.h>

#define CALL //printf
#define INFO //printf
#define ERROR printf
#define WARNING printf
#define DEBUG //printf
#define BUFFER //mprintf

FBufferReclaimer::FBufferReclaimer( BBufferGroup *group )
	: mGroup( group )
	{
	Start( B_LOW_PRIORITY );
}

void FBufferReclaimer::ThreadFunction() {
	BUFFER(">>> Reclaiming Buffers\n");
	mGroup->ReclaimAllBuffers();
	BUFFER(">>> Waiting for Buffers\n");
	mGroup->WaitForBuffers();
	BUFFER(">>> Buffers reclaimed.\n");
	delete mGroup;
	delete this; // !!
}

FBufferManager::FBufferManager( const char *name, int32 abuffers ) {
	CALL("FBufferManager::ctor( %s )\n", name );

	mName = strdup( name );
	mBufferGroup = NULL;
	mOwnBuffers = false;
	nBuffers = 0;
	aBuffers = abuffers;
	mBuffer = new BBuffer*[ abuffers ];
}

FBufferManager::~FBufferManager() {
	CALL("FBufferManager::dtor()\n");

	if( mOwnBuffers && nBuffers && mBufferGroup ) {
		INFO("FBufferManager::dtor: Reclaiming Buffers\n");
		status_t err;
		if( B_OK != (err = mBufferGroup->ReclaimAllBuffers()) ) {
			ERROR("%s::dtor: ReclaimBuffers: %s\n", mName, strerror( err ) );;
		}
		delete mBufferGroup; // will delete contained buffers (really?)
	}
	if( mBuffer ) delete mBuffer;
	if( mName ) delete mName;
}

status_t FBufferManager::UseBuffers( const media_format& format, BBufferGroup *group ) {
	ReclaimBuffers();

	CALL("FBufferManager::UseBuffers()\n");
	
	mBufferGroup = group;
	
	int32 n;
	mBufferGroup->CountBuffers( &n );
	if( n > aBuffers ) Reallocate( n+3 );
	nBuffers = n;
	
	mBufferGroup->GetBufferList( nBuffers, mBuffer );
	
	for( int i=0; i<nBuffers; i++ ) {
		INFO("Buffer[%i] %p\n", i, mBuffer[i] );
	}
	
	mOwnBuffers = false;
	
	return( B_OK );
}

status_t FBufferManager::CreateBuffers( const media_format& format, int32 n, size_t sz ) {
	if( n > aBuffers ) Reallocate( n+3 );
	nBuffers = n;
	mOwnBuffers = true;
	if( !sz ) sz = FMediaIdealBufferSizeForFormat( &format );
	mBufferGroup = new BBufferGroup( sz, nBuffers );
	mBufferGroup->GetBufferList( nBuffers, mBuffer );
	INFO("%s: Created %i buffers of size %i: %s\n", Name(), nBuffers, sz, strerror( mBufferGroup->InitCheck() ) );
	return( mBufferGroup->InitCheck() );
}

int32 FBufferManager::IndexForBuffer( BBuffer* buffer ) {
	CALL("FBufferManager::IndexForBuffer()\n");
	
	INFO("Finding Buffer %p in %s: ", buffer, mName );
	for( int i=0; i<nBuffers; i++ ) {
		INFO("Buffer %i: %p\n", i, mBuffer[i] );
		if( mBuffer[i] == buffer ) return( i );
	}
	INFO("Not found\n");
	return -1;
}

void FBufferManager::SyncIn( int32 index ) {
}

void FBufferManager::SyncOut( int32 index ) {
}

void FBufferManager::ReclaimBuffers() {
	if( mOwnBuffers ) {
		new FBufferReclaimer( mBufferGroup );

/*
		CALL("FBufferManager::ReclaimBuffers()\n");
		mBufferGroup->ReclaimAllBuffers();
		delete mBufferGroup;
*/
		mBufferGroup = NULL;
		mOwnBuffers = false;
	} else {
		CALL("FBufferManager::ReclaimBuffer( Foreign )\n");
	}
	mBufferGroup = NULL;
	nBuffers = 0;
}

status_t FBufferManager::Identify( BBuffer *Buffer ) {
	int i = IndexForBuffer( Buffer );
	if( i < 0 || i > nBuffers ) return B_ERROR;
	
	RegisterMetaHeader( i );
	return B_OK;
}

void FBufferManager::RegisterMetaHeader( int i ) {
	mBuffer[i]->Header()->user_data_type = F_META_HEADER_TYPE;
	FMetaHeader *mHeader = (FMetaHeader*)(&(mBuffer[i]->Header()->user_data));
	mHeader->Bitmap = NULL;
	mHeader->View = NULL;
}

void FBufferManager::Reallocate( int32 a ) {
	if( a > aBuffers ) {
		BBuffer **newBuffers = new BBuffer*[ a ];
		for( int i=0; i<nBuffers; i++ )
			newBuffers[i] = mBuffer[i];
		for( int i=nBuffers; i<a; i++ )
			newBuffers[i] = NULL;
		aBuffers = a;
		delete mBuffer;
		mBuffer = newBuffers;
	}
}

BBuffer *FBufferManager::GetBuffer( size_t size_needed, bigtime_t timeout ) {
	BBufferGroup *bGroup = mBufferGroup;
	if( !bGroup ) {
		WARNING("%s: No BufferGroup. Call CreateBuffers somewhere, or use F_SIMPLE_BUFFERS outputs.\n", Name());
		return NULL;
	}

	INFO("%s: Requesting Buffer size %i\n", mName, size_needed);
	BBuffer *Buffer = bGroup->RequestBuffer( size_needed, timeout );
	if( !Buffer ) {
		status_t err = bGroup->RequestError();
		if( err == B_TIMED_OUT ) {
			printf("."); fflush(stdout);
		} else if( err == B_MEDIA_BUFFERS_NOT_RECLAIMED ) {
			WARNING("%s: Reclaiming Buffers\n", mName );
			ReclaimBuffers();
		} else {
			WARNING("%s: Couldn't get Buffer (%s).\n", mName, strerror( err ));
		}
		return NULL;
	} else if( Buffer->SizeAvailable() < size_needed ) {
		WARNING("%s: Didn't get large enough buffer (%i bytes missing)\n", mName, size_needed - Buffer->SizeAvailable());
		Buffer->Recycle();
		return NULL;
	}

	INFO("Got Buffer\n" );

	return Buffer;
}
