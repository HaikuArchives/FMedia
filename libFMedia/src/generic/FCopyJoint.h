#ifndef F_FCopyJoint_H
#define F_FCopyJoint_H

#include "FControllable.h"
#include "FBufferManager.h"

//! joins two channels into a new one.
/*!
	
*/

class FCopyJoint : public FControllable {
	public:
		FCopyJoint( const media_format& preferredFormat, const media_format& requiredFormat, const char *name, BMediaAddOn *addon = NULL );
		virtual ~FCopyJoint();
				
	protected:
		// FMediaNode
		virtual void InputConnected( FMediaInput *input, bool connected = true );
		virtual bool HandleBuffer( BBuffer *Buffer, FMediaInput *onInput );
		virtual status_t FormatChanged( FMediaInput *input );

	protected:

		virtual void Setup();

//! hook for your filter function
		virtual f_filter_status Filter( BBuffer *inA, BBuffer *inB, BBuffer *out ) = 0;

		
	protected:
		media_format mRequiredFormat, mPreferredFormat, mOutputFormat;

		FMediaInput *mInputA, *mInputB;
		FMediaOutput *mOutputA, *mOutputB, *mOutput;

		BBuffer *mHoldB;

	private:
};

#endif
