#ifndef F_FJoint_H
#define F_FJoint_H

#include "FControllable.h"
#include "FBufferManager.h"
#include "FBufferBuffer.h"

//! joins two channels into one.
/*!
	
*/
class FJoint : public FControllable {
	public:
		FJoint( const media_format& preferredFormat, const media_format& requiredFormat, const char *name, BMediaAddOn *addon = NULL );
		virtual ~FJoint();
				
	protected:
		// FMediaNode
		virtual void InputConnected( FMediaInput *input, bool connected = true );
		virtual bool HandleBuffer( BBuffer *Buffer, FMediaInput *onInput );
		virtual status_t FormatChanged( FMediaInput *input );

	protected:

		virtual void Setup();

//! hook for your filter function
		virtual bool Filter( BBuffer *inA, BBuffer *inB ) = 0;

		
	protected:
		media_format mRequiredFormat, mPreferredFormat, mOutputFormat;

		FMediaInput *mInputA, *mInputB;
		FMediaOutput *mOutputA, *mOutputB;

		FBufferBuffer mHold;
		BBuffer *mHoldA;

	private:
};

#endif
