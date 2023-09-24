#ifndef F_FBufferBuffer_H
#define F_FBufferBuffer_H

#include <Buffer.h>

class FBufferBuffer {
	public:
		FBufferBuffer( int32 _n = 3 ) {
				n = _n;
				c = 0;
				mBuffers = new BBuffer*[n];
				for( int i=0; i<n; i++ ) {
					mBuffers[i] = NULL;
				}
			}
			
		void Add( BBuffer *buffer ) {
				c++;
				if( c >= n ) c = 0;
				if( mBuffers[c] ) mBuffers[c]->Recycle();
				mBuffers[c] = buffer;
			}
			
		BBuffer *Get( bigtime_t after ) {
				for( int i=c+1; i!=c; i++ ) {
					if( i>=n ) i=0;
					if( mBuffers[i] == NULL ) {
						return( NULL );
					} else if( mBuffers[i]->Header()->start_time >= after || i==c ) {
						BBuffer *b = mBuffers[i];
						mBuffers[i] = NULL;
						return( b );
					} else {
						mBuffers[i]->Recycle();
						mBuffers[i] = NULL;
					}
				}
				return( NULL );
			}
		
	protected:
		int32 n;
		int32 c;
		BBuffer **mBuffers;
};

#endif
