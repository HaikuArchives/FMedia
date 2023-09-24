#ifndef F_VIEW_BUFFER_MANAGER_H
#define F_VIEW_BUFFER_MANAGER_H

#include <Bitmap.h>
#include <View.h>
#include "FBufferManager.h"

class FViewBufferManager : public FBufferManager {
	public:
		FViewBufferManager( const char *name = "FViewBufferManager" );
		virtual ~FViewBufferManager();
	
		virtual status_t CreateBuffers( const media_format& format, int32 n = 3, size_t sz = 0  );
		virtual status_t UseBuffers( const media_format& format, BBufferGroup *group );
	
		virtual void SyncIn( int32 i ) { if( !mOwnBuffers ) Copy( mBuffer[i], mBitmap[i] ); }
		virtual void SyncOut( int32 i ) { if( !mOwnBuffers ) { mView[i]->Sync(); Copy( mBuffer[i], mBitmap[i] ); } }
		virtual void SyncIn( int32 i, BBuffer *buffer ) { if( !mOwnBuffers ) Copy( buffer, mBitmap[i] ); }
	
		virtual void RegisterMetaHeader( int i );

		BBitmap *BitmapAt( int32 i ) { return mBitmap[i]; }
		BView *ViewAt( int32 i ) { return mView[i]; }
		BView *ViewFor( BBuffer *buffer ) { int i=IndexForBuffer( buffer ); return i==-1 ? NULL : ViewAt( i ); }
		
	protected:
		status_t CreateBitmaps( const media_raw_video_format* format, uint32 flags=0 );

		BBitmap **mBitmap;
		BView **mView;

	public:
		static void Copy( BBuffer *buffer, BBitmap *bitmap );
		static void Copy( BBitmap *bitmap, BBuffer *buffer );
};

#endif
