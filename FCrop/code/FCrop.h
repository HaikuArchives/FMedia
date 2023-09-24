#ifndef F_FCrop_H
#define F_FCrop_H

#include "FCopyFilter.h"
#include "FParameter.h"
#include "esoteric.h"

class FCrop : public FCopyFilter {
	public:
		FCrop( const char *name = ADDON_NAME, BMediaAddOn *addon = NULL );
		~FCrop();
				
		// FCopyFilter
		virtual void Setup();
		virtual bool Filter( BBuffer *thruBuffer, BBuffer *outBuffer );
		virtual status_t AdaptFormat( media_format* inputFormat );
		
	protected:
		virtual void ParameterChanged( int32 id );  // (optional)
		FContinuousParameter *mLeft, *mTop, *mWidth, *mHeight;
		int oldW, oldH;
};

#endif
