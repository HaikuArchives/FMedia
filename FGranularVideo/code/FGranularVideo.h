#ifndef F_FGranularVideo_H
#define F_FGranularVideo_H

#include "FFilter.h"
#include "FParameter.h"
#include "esoteric.h"
#include "FVideoGranule.h"

class FGranuleControlView;

class FGranularVideo : public FFilter {
	public:
		FGranularVideo( const char *name = ADDON_NAME, BMediaAddOn *addon = NULL );
		~FGranularVideo();
	
	protected:			
		virtual void Setup();
		virtual bool Filter( BBuffer *inBuffer );

		virtual void FilterFormatChanged( const media_format& new_format );
		virtual void ParameterChanged( FParameter *param );  // (optional)

		friend class FGranuleControlView;
		FGranuleControlView *mView;
		
		FContinuousParameter *mGranulationThreshold;

		FVideoGranule **mGranules;
		int nGranules;
		int playGranule;
		int recordGranule;
		bool recording;
		int recFrames;
		
		FVideoGranule *Recorder;
		
		bool *mPatchVisible;
		BRect *mPatchArea;
		int nPatches;

		int mSkip;

		void Fill( int32 *dst, int32 *src );
		
		bool thru;
};

#endif
