#ifndef F_FGranuleControl_H
#define F_FGranuleControl_H

#include <Window.h>
#include <View.h>

#include "FGranularVideo.h"

class FGranuleControlView : public BView {
	public:
		FGranuleControlView( int32 ngranules, FGranularVideo *node );
		~FGranuleControlView();
		
//		virtual void MessageReceived( BMessage *msg ); 
		virtual void Draw( BRect updateRect );
		
		virtual void KeyDown( const char *bytes, int32 numBytes );
		
		void Close() {
				mWindow->Lock();
				mWindow->Quit();
			}
		
		void Redraw() {
				if( mWindow->Lock() ) {
					Invalidate( Bounds() );
					mWindow->Unlock();
				}
			}
			
		void Pulse() {
				mWindow->Activate( true );
			}
		
	protected:
		int32 nGranules;
		FGranularVideo *mNode;
		
		BWindow *mWindow;
		
		bool quitting;
};

#endif
