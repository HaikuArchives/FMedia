#ifndef F_BITMAP_BUFFER_MANAGER_H
#define F_BITMAP_BUFFER_MANAGER_H

#include <Bitmap.h>
#include "FBufferManager.h"

class FBitmapBufferManager : public FBufferManager {
	public:
		FBitmapBufferManager( const char *name = "FBitmapBufferManager" );
		~FBitmapBufferManager();
	
		virtual status_t CreateBuffers(  media_format& format, int32 n );
		virtual status_t UseBuffers( media_format& format, BBufferGroup *group );
	
		virtual void SyncIn( int32 i ) { if( !mOwnBuffers ) Copy( mBuffer[i], mBitmap[i] ); }
		virtual void SyncOut( int32 i ) { if( !mOwnBuffers ) Copy( mBuffer[i], mBitmap[i] ); }
	
		BBitmap *BitmapAt( int32 i ) { return mBitmap[i]; }

		virtual void RegisterMetaHeader( int i );
		
	protected:
		status_t CreateBitmaps( media_raw_video_format* format, uint32 flags=0 );

		BBitmap **mBitmap;

	public:
		static void Copy( BBuffer *buffer, BBitmap *bitmap );
		static void Copy( BBitmap *bitmap, BBuffer *buffer );
};

#endif
