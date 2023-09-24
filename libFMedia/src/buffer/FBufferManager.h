#ifndef F_BUFFER_MANAGER_H
#define F_BUFFER_MANAGER_H

#include <BufferGroup.h>
#include <Buffer.h>
#include "FMediaDefs.h"
#include "FSimpleThread.h"

class BBitmap;
class BView;

class FBufferReclaimer : public FSimpleThread {
	public:
		FBufferReclaimer( BBufferGroup *group );
		
		virtual void ThreadFunction();
		
	protected:
		BBufferGroup *mGroup;
};

class FBufferManager {
	public:
		FBufferManager( const char *name, int32 abuffers = 5 );
		virtual ~FBufferManager();
	
		virtual status_t UseBuffers( const media_format& format, BBufferGroup *group );
		virtual status_t CreateBuffers( const media_format& format, int32 n = 3, size_t sz = 0 );

		BBufferGroup *Group() { return mBufferGroup; }
		bool OwnsBuffers() { return mOwnBuffers; }
		const char *Name() { return mName; }
		
		int32 IndexForBuffer( BBuffer *buffer );
		BBuffer *BufferAt( int32 i ) { return mBuffer[i]; }

		virtual void SyncIn( int32 index );
		virtual void SyncOut( int32 index );
		void ReclaimBuffers();
		
		virtual status_t Identify( BBuffer *Buffer );
		virtual void RegisterMetaHeader( int i );

		BBuffer *GetBuffer( size_t size_needed, bigtime_t timeout = 50000 ); // might return NULL!
		
		int32 CountBuffers() { return nBuffers; }

	protected:
		void Reallocate( int32 a );
	
		BBufferGroup *mBufferGroup;	
		bool mOwnBuffers;
		
		int32 nBuffers;		// number of buffers
		int32 aBuffers;		// space for BBuffer*s allocated in mBuffer
		BBuffer **mBuffer;
		
		char *mName;
};

struct FMetaHeader {
	BBitmap *Bitmap;
	BView *View;
};

#endif
