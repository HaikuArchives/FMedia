#include "FStatus.h"
#include "FMedia.h"

#include <Box.h>
#include <Application.h>
#include <Resources.h>
#include <Application.h>
#include <Bitmap.h>
#include <Mime.h>
#include <FindDirectory.h>
#include <Path.h>

#include <stdio.h>
#include <string.h>

///////////////////////////////////////////
// FStatusIcon

BBitmap *FStatusIcon::ErrorIcon = NULL;
BBitmap *FStatusIcon::WarningIcon = NULL;
BBitmap *FStatusIcon::InfoIcon = NULL;

rgb_color gray = { 0xd0, 0xd0, 0xd0 };

FStatusIcon::FStatusIcon()
	: BView( BRect( 0, 0, 16, 16 ), "StatusIcon", B_FOLLOW_TOP | B_FOLLOW_LEFT, B_WILL_DRAW ) {
	LoadIcons();
	Icon = NULL;
	Status = none;
}

void FStatusIcon::SetStatus( StatusT status ) {
	if( status == Status ) return;

	Status = status;

	switch( Status ) {
		case ok:
			Icon = InfoIcon;
			break;
		case warning:
			Icon = WarningIcon;
			break;
		case error:
			Icon = ErrorIcon;
			break;
		default:
			Icon = NULL;
			break;
	}
	
	if( Window() && Window()->Lock() ) {
		Invalidate( Bounds() );
		Window()->Unlock();
	}
}

void FStatusIcon::Draw( BRect updateRect ) {
	if( !Icon ) return;
	SetDrawingMode( B_OP_OVER );
	DrawBitmap( Icon, BPoint( 0, 0 ) );
}

void FStatusIcon::LoadIcons() {
	static bool tried = false;
	
	if( !tried ) {
		tried = true;
		
		BPath Path;
		
		find_directory( B_USER_SETTINGS_DIRECTORY, &Path );
		Path.Append(F_MEDIA_SETTINGS_PATH);
		BFile File( Path.Path(), B_READ_ONLY );
		if( File.InitCheck() == B_OK )
		{
			BResources res;
			status_t err;
			if( res.SetTo(&File) == B_OK ) {
				void *data;
				size_t len;

				// Error Icon
				data = res.FindResource( 0x4d49434e, "BEOS:M:error", &len );
				if( data ) {
					ErrorIcon = new BBitmap( BRect( 0, 0, B_MINI_ICON-1, B_MINI_ICON-1 ), B_COLOR_8_BIT );
					if( len != ErrorIcon->BitsLength() ) ErrorIcon = NULL;
					else {
						memcpy( ErrorIcon->Bits(), data, len );
					}
				}

				// Warning Icon
				data = res.FindResource( 'MICN', "BEOS:M:warning", &len );
				if( data ) {
					WarningIcon = new BBitmap( BRect( 0, 0, B_MINI_ICON-1, B_MINI_ICON-1 ), B_COLOR_8_BIT );
					if( len != WarningIcon->BitsLength() ) WarningIcon = NULL;
					else {
						memcpy( WarningIcon->Bits(), data, len );
					}
				}
				
				// Info Icon
				data = res.FindResource( 'MICN', "BEOS:M:info", &len );
				if( data ) {
					InfoIcon = new BBitmap( BRect( 0, 0, B_MINI_ICON-1, B_MINI_ICON-1 ), B_COLOR_8_BIT );
					if( len != InfoIcon->BitsLength() ) InfoIcon = NULL;
					else {
						memcpy( InfoIcon->Bits(), data, len );
					}
				}
			}
		}
	}
}

///////////////////////////////////////////
// FStatus

FStatus::FStatus( const char *label ) 
	: BView( BRect( 0, 0, 200, 25 ), label, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP, 0 ) {
	
	SetViewColor( gray );
	
	Icon = new FStatusIcon();
	Icon->MoveTo( 6, 6 );

	float divider = 75;
	
	BRect r = Bounds();
	r.left += 24;
	r.right = r.left + divider;
	r.InsetBy( 4, 4 );
	r.bottom -= 1;
	r.top -= 1;
	Label = new BStringView( r, "Status Label", label );

	r = Bounds();
	r.left = 24 + divider;
	r.InsetBy( 4, 4 );
	r.bottom -= 1;
	r.top -= 1;
	Status = new BStringView( r, "Status", "-" );
	
	r = Bounds();
	r.InsetBy( -1, -1 );
	BBox *box = new BBox( r );
	
	Icon->SetViewColor( gray );
	box->AddChild( Icon );
	Label->SetViewColor( gray );
	box->AddChild( Label );
	Status->SetViewColor( gray );
	box->AddChild( Status );
	box->SetViewColor( gray );
	AddChild( box );
}

void FStatus::SetValue( const char *value ) {
	if( Window() && Window()->Lock() ) {
		Status->SetText( value );
		Icon->SetStatus( FStatusIcon::ok );
		Window()->Unlock();
	}
}

void FStatus::SetValue( const char *value, FStatusIcon::StatusT status ) {
	if( Window() && Window()->Lock() ) {
		Status->SetText( value );
		Icon->SetStatus( status );
		Window()->Unlock();
	}
}

void FStatus::SetValue( float value, const char *unit ) {
		char *Text = new char[ 0xff ];
			sprintf( Text, "%.2f %s", value, unit );
			SetValue( Text );
		delete Text;
		}

void FStatus::SetValue( int value, const char *unit ) {
	char *Text = new char[ 0xff ];
		sprintf( Text, "%d %s", value, unit );
		SetValue( Text );
	delete Text;
}

void FStatus::SetValue( status_t value ) {
	switch( value ) {
		case B_ERROR:
			SetValue( "General Error" );
			SetStatus( FStatusIcon::warning );
			break;
		case B_OK:
			SetValue( "OK" );
			SetStatus( FStatusIcon::ok );
			break;
		default:
			SetValue( strerror( value ) );
			SetStatus( FStatusIcon::error );
			break;
	}
}

void FStatus::SetStatus( FStatusIcon::StatusT status ) {
	Icon->SetStatus( status );
}

///////////////////////////////////////////
// FTimeStatus

FTimeStatus::FTimeStatus( const char *label, bigtime_t min, bigtime_t max )
	: FStatus( label ) {
	
	mMin = min;
	mMax = max;
}

void FTimeStatus::SetValue( bigtime_t value ) {

	if( abs(value) > 1000000 ) {
		FStatus::SetValue( ((float)value)/1000000, "s" );
	} else if( abs(value) > 1000 ) {
		FStatus::SetValue( ((float)value)/1000, "ms" );
	} else {
		FStatus::SetValue( (int)value, "µs" );
	}

	FStatusIcon::StatusT status;
	if( ( value < mMin && mMin != -1 )
			|| ( value > mMax && mMax != -1 ) )
		status = FStatusIcon::warning;
	else status = FStatusIcon::ok;
	SetStatus( status );
	
}

///////////////////////////////////////////
// FStatusWindow

FStatusWindow::FStatusWindow( const char *label )
	: FStateSaveWindow( BRect( 430, 50, 630, 0 ), label, B_TITLED_WINDOW, B_NOT_CLOSABLE | B_NOT_ZOOMABLE | B_NOT_RESIZABLE ) {
	bottom = -1;
	SetSizeLimits( 10, 1000, 10, 1000 );
}

void FStatusWindow::AddStatus( FStatus *status ) {
	bottom++;
	BRect myBounds = Bounds();
	BRect sBounds = status->Bounds();
	status->MoveTo( 0, bottom );
	status->ResizeTo( myBounds.Width(), sBounds.Height() );
	
	bottom += sBounds.Height();
	
	ResizeTo( myBounds.Width(), bottom );
	AddChild( status );
}
