#ifndef F_FControlDataNode_H
#define F_FControlDataNode_H

#include "FControllable.h"

// for mode
#define F_CONTROLDATA_NONE	0
#define F_CONTROLDATA_IN	0x1
#define F_CONTROLDATA_OUT	0x2
#define F_CONTROLDATA_THRU	F_CONTROLDATA_IN | F_CONTROLDATA_OUT

struct FControlDataParameter {
	FParameter		*Parameter;
	FMediaInput 	*Input;
	FMediaOutput 	*Output;
	
	FControlDataParameter( FParameter *p = NULL, FMediaInput *in = NULL, FMediaOutput *out = NULL ) 
		: Parameter(p), Input(in), Output(out) { ; }
};

class FControlDataNode : public FControllable {
	public:
		FControlDataNode( const char *name, media_type in_type, media_type out_type, BMediaAddOn *addon );
		~FControlDataNode();

		virtual void Setup();
		
//! handle control data buffers
		virtual bool HandleBuffer( BBuffer *inBuffer, FMediaInput *onInput  );
//! will send a buffer when an "outputted" parameter was changed.
/*!
	/todo timing??
*/
		virtual void ParameterChanged( FParameter *parameter );

//! Adds parameter and inputs/outputs
/*!
	/param mode one of F_CONTROLDATA_NONE, F_CONTROLDATA_IN, F_CONTROLDATA_OUT or F_CONTROLDATA_THRU
*/
		int32 AddControlDataParameter( FParameter *Parameter, int mode );

//! Sets parameter name according to Format MetaData
		virtual void InputConnected( FMediaInput *input, bool connected=false );
		
	protected:
		FArray<FControlDataParameter *> mControlDataParameters;
	
	private:
		typedef FControllable inherited;
};

#endif
