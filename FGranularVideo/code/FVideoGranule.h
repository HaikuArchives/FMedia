#ifndef F_FVideoGranule_H
#define F_FVideoGranule_H

#include "FMediaFormats.h"

class FVideoGranule {
	public:
		FVideoGranule( size_t frameSize, int maxFrames );
		~FVideoGranule();
		
		void AddFrame( int32 *data );

		int32 *GetNextFrame( bigtime_t now ) {
				if( forward ) {
					cFrame++;
					if( cFrame >= nFrames ) {
						forward = false;
						cFrame--;
					}
				} else {
					cFrame--;
					if( cFrame <= 0 ) {
						forward = true;
						cFrame++;
					}
				}
				lastPlayed = now;
				isNew = false;
				return mFrames[cFrame];
			}
			
		void Reset() {
				cFrame = 0;
				forward = true;
			}
			
		bigtime_t LastPlayed() {
				return lastPlayed;
			}
			
		bool IsNew() {
				return isNew;
			}
			
		int Length() {
				return( nFrames );
			}

	protected:
		int32 **mFrames;
		int nFrames;
		int aFrames;
		int cFrame;
		size_t mFrameSize;
		
		bool forward;
		bigtime_t lastPlayed;
		bool isNew;
};

#endif
