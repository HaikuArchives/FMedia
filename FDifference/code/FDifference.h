#ifndef F_FDifference_H
#define F_FDifference_H

#include "FFilter.h"
#include "FControllable.h"
#include "FParameter.h"
#include "esoteric.h"
#include "FPixel.h"

class FDifference : public FFilter {
	public:
		FDifference( const char *name = ADDON_NAME, BMediaAddOn *addon = NULL );
		~FDifference();
				
		// FCopyFilter
		void Setup(); 
		virtual bool Filter( BBuffer *thruBuffer );

	protected:
		FContinuousParameter *mFactor;
		FContinuousParameter *mThreshold;

		FDiscreteParameter *mShow;
		FDiscreteParameter *mRough;

		BBuffer *holdBuffer;
};

#endif
