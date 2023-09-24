#ifndef F_FStateSaveWindow_H
#define F_FStateSaveWindow_H

#include <Window.h>
#include "FState.h"

class FStateSaveWindow : public BWindow {
	public:
		FStateSaveWindow( BRect defaultRect, const char *title, window_type type, uint32 flags, uint32 workspace = B_CURRENT_WORKSPACE );
		~FStateSaveWindow();
	
		virtual void FrameMoved( BPoint pos );
		virtual void FrameResized( float w, float h );
			
	protected:
		FState<BRect> mFrame;
};

#endif
