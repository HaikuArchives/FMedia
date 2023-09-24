#include "FGranuleControl.h"

#define DEBUG 1
#include <Debug.h>

#define GRANSIZE 11
#define EXTRA_INFO 2

rgb_color GranBG = { 0, 0, 0 };
rgb_color GranEmpty = { 0xaa, 0xaa, 0xaa };
rgb_color GranFull = { 0xdd, 0xdd, 0xdd };
rgb_color GranRec = { 0xaa, 0, 0 };
rgb_color GranNew = { 0, 0, 0xaa };
rgb_color GranPlay = { 0, 0xaa, 0 };

rgb_color PatchBG = { 0xaa, 0xaa, 0xaa };
rgb_color PatchPlay = { 0xdd, 0xdd, 0xdd };

rgb_color cRecord = { 0xaa, 0, 0 };
rgb_color cPause = { 0xaa, 0xaa, 0xaa };

FGranuleControlView::FGranuleControlView( int32 ngranules, FGranularVideo *node ) 
	: nGranules( ngranules )
	, mNode( node )
	, BView( BRect( 0, 0, 1+((ngranules+EXTRA_INFO)*GRANSIZE), 1+GRANSIZE ), "Granule Control", B_FOLLOW_ALL_SIDES, B_WILL_DRAW | B_PULSE_NEEDED )
	{
	
	mWindow = new BWindow( BRect( 1, 1, 1+((ngranules+EXTRA_INFO)*GRANSIZE), 1+GRANSIZE ), "GranuleControl", B_NO_BORDER_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL, 0 );
	mWindow->AddChild( this );
	mWindow->Show();
	
	mWindow->Lock();
	SetViewColor( B_TRANSPARENT_32_BIT );
	SetHighColor( GranBG );
	FillRect( Bounds() );
	MakeFocus( true );
	mWindow->SetPulseRate( 1000000 );
	mWindow->Unlock();
	
	quitting = false;
}

FGranuleControlView::~FGranuleControlView() {
}

/*
void FGranuleControlView::MessageReceived( BMessage *msg ) {
				PRINT_OBJECT( *msg );
	printf("msg\n");
	switch( msg->what ) {
		case B_KEY_DOWN: {
				int32 key;
				if( B_OK == msg->FindInt32("key", &key ) ) {
					printf("Key: %i\n", key );
					switch( key ) {
					}
				}
			} break;
		default:
			BView::MessageReceived( msg );
	}

}
*/

void FGranuleControlView::KeyDown( const char *bytes, int32 numBytes ) {
	if( bytes[0] >= '1' && bytes[0] < '1'+nGranules ) {
		int g = bytes[0] - '1';
		if( g != mNode->recordGranule && mNode->mGranules[g] != NULL ) {
			mNode->mGranules[g]->Reset();
			mNode->playGranule = g;
			mNode->thru = false;
		}
		quitting = false;
	} else {
		switch( bytes[0] ) {
			case '0':
				mNode->thru = true;
			case 'q':
//				if( quitting ) Close();
//				else quitting = true;
				break;
			case 'u':
				mNode->mPatchVisible[0] = !mNode->mPatchVisible[0];
				break;
			case 'i':
				mNode->mPatchVisible[1] = !mNode->mPatchVisible[1];
				break;
			case 'o':
				mNode->mPatchVisible[2] = !mNode->mPatchVisible[2];
				break;
			case 'j':
				mNode->mPatchVisible[3] = !mNode->mPatchVisible[3];
				break;
			case 'k':
				mNode->mPatchVisible[4] = !mNode->mPatchVisible[4];
				break;
			case 'l':
				mNode->mPatchVisible[5] = !mNode->mPatchVisible[5];
				break;
			case 'm':
				mNode->mPatchVisible[6] = !mNode->mPatchVisible[6];
				break;
			case ',':
				mNode->mPatchVisible[7] = !mNode->mPatchVisible[7];
				break;
			case '.':
				mNode->mPatchVisible[8] = !mNode->mPatchVisible[8];
				break;
			case B_UP_ARROW:
				mNode->mGranulationThreshold->Set( mNode->mGranulationThreshold->Get() + .5 );
				break;
			case B_DOWN_ARROW:
				mNode->mGranulationThreshold->Set( mNode->mGranulationThreshold->Get() - .5 );
				break;
			case B_SPACE:
				mNode->mSkip+=5;
				break;
			default:
//				printf("Other Key: %i\n", bytes[0] );
				quitting = false;
		}
	}
	Redraw();
}

void FGranuleControlView::Draw( BRect updateRect ) {

	SetHighColor( GranBG );
	float x = 0;
	for( int i=0; i<nGranules+EXTRA_INFO; i++ ) {
		StrokeRect( BRect( x, 0, x+GRANSIZE, GRANSIZE ) );
		x+=GRANSIZE;
	}	

	// draw granuleinfo
	x = 1;
	for( int i=0; i<nGranules; i++ ) {
		if( mNode->mGranules[i] && mNode->mGranules[i]->IsNew() ) {
			SetHighColor( GranNew );
		} else if( i == mNode->playGranule ) {
			SetHighColor( GranPlay );
		} else if( mNode->mGranules[i] ) {
			SetHighColor( GranFull );
		} else {
			SetHighColor( GranEmpty );
		}
		FillRect( BRect( x, 1, x+GRANSIZE-2, GRANSIZE-1 ) );
		x+=GRANSIZE;
	}

	// draw patches
	int p=0;
	int xofs = x;
	int w = 3;

	for( int y=0; y<3; y++ ) {
		for( int x=0; x<3; x++ ) {
			if( mNode->mPatchVisible[p] ) {
				SetHighColor( PatchPlay );
			} else {
				SetHighColor( PatchBG );
			}
			FillRect( BRect( xofs+(x*w), (y*w)+1, xofs+(x*w)+w, (y*w)+w+1 ) );
			p++;
		}
	}

	// draw rec info
	xofs+=GRANSIZE;
	if( mNode->recording ) {
		SetHighColor( cRecord );
	} else {
		SetHighColor( cPause );
	}
	FillRect( BRect( xofs, 1, xofs+GRANSIZE-2, GRANSIZE-1 ) );
	

	mWindow->Activate( true );

}
