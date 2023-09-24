#ifndef F_FStatus_H
#define F_FStatus_H

#include "FStateSaveWindow.h"
#include <View.h>

#include <StringView.h>

class FStatusIcon : public BView {
	public:
		enum StatusT { none, ok, warning, error };
		
		FStatusIcon();
		void SetStatus( StatusT status );
		
		virtual void Draw( BRect updateRect );

	protected:
		BBitmap* Icon;
		StatusT Status;

		// Icons
		static BBitmap* ErrorIcon;
		static BBitmap* WarningIcon;
		static BBitmap* InfoIcon;
		static void LoadIcons();
};

class FStatus : public BView {
	public:
		FStatus( const char *label );
		
		virtual void SetValue( const char *value );
		virtual void SetValue( const char *value, FStatusIcon::StatusT status );
		virtual void SetValue( float value, const char *unit );
		virtual void SetValue( int value, const char *unit );
		virtual void SetValue( status_t value );
		
		virtual void SetStatus( FStatusIcon::StatusT status );

	protected:
		FStatusIcon *Icon;
		BStringView *Label, *Status;
};

class FTimeStatus : public FStatus {
	public:
		FTimeStatus( const char *label, bigtime_t min=0, bigtime_t max=1000000 );
		
		virtual void SetValue( bigtime_t value );
		virtual void SetLimits( bigtime_t min, bigtime_t max ) { mMin = min; mMax = max; }
		
	protected:
		bigtime_t mMin, mMax;
};

class FStatusWindow : public FStateSaveWindow {
	public:
		FStatusWindow( const char *Label );
		
		void AddStatus( FStatus *status );
		
	protected:
		float bottom;
};

#endif
