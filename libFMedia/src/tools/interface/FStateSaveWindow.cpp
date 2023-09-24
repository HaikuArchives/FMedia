#include "FStateSaveWindow.h"

#include <FindDirectory.h>
#include <Path.h>
#include <Entry.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "FMedia.h"

/////////////////////////////////////////////////////// FStateSaveWindow

FStateSaveWindow::FStateSaveWindow( BRect defaultRect, const char *title, window_type type, uint32 flags, uint32 workspace )
	: BWindow( defaultRect, title, type, flags, workspace ),
	  mFrame( title, B_RECT_TYPE, "WindowStates" ) {
	  
	  BRect r = mFrame.Get( defaultRect );
	  MoveTo( r.left, r.top );
	  ResizeTo( r.Width(), r.Height() );
}


FStateSaveWindow::~FStateSaveWindow() {
	mFrame.Put( Frame() );
}

void FStateSaveWindow::FrameMoved( BPoint pos ) {
//	mFrame.Put( Frame() );
}

void FStateSaveWindow::FrameResized( float w, float h ) {
//	mFrame.Put( Frame() );
}
