#ifndef F_FFilter_H
#define F_FFilter_H

#include "FControlDataNode.h"

//! a simple "thru" media filter that emits data in the same format as it takes them.
/*!
	FFilter allows you to implement a simple "thru" Filter node.
	Derive from it,
	set requiredFormat and preferredFormat from your constructor
	and implement Filter() to do your thing (and call FMediaNode::SendOrRecycleBuffer() ).
	If you need to do something (like allocate memory) when the final format
	is set or changed, also implement FormatChanged().
	
	To have y'r own Parameters, use FControllable's functions. You might also want
	to somehow access/use mInput and mOutput, if not only in Filter().
*/

class FFilter : public FControlDataNode {
	public:
		FFilter( const media_format& requiredFormat, const media_format& preferredFormat, const char *name, BMediaAddOn *addon = NULL );
		virtual ~FFilter();

	protected:

//! the main filter function. override this to do your thing
/*!
	\param inBuffer the BBuffer to process. inBuffer->Data() points to the actual data of size inBuffer->SizeUsed().

	currently, the return code decides on wether the time this function takes to executed
	is a valid measurement and "counted" for latency adaption.
	
	You should either call SendOrRecycleBuffer() with inBuffer, mOutput and mInput,
	or RecycleBuffer() with inBuffer and mInput to take care you won't eat them buffers all up.
	(This might be coupled to the return code one day.)	
*/
		virtual bool Filter( BBuffer *inBuffer ) = 0;

//! called when the handled format is (re)established
/*!
	the new_format will always be "acceptable" in respect to mRequiredFormat, but might
	be more specialized.
	Called once after the input has been connected, and ever after when a format
	change travels its way thru the graph.
*/
		virtual void FilterFormatChanged( const media_format& new_format );

//! Adds mInput
/*!
	Adds a ("the") input to the node, the corresponding output will be created when it is being connected.
	The input will assure the required format, possibly specializing to the preferred one.
	Make sure you call inherited Setup()s for your direct parents when you override it.
*/
		virtual void Setup();
		
	private:				
		// FMediaFilter
//! FMediaFilter::HandleBuffer() -> Filter() wrap
/*!
	When onInput matches mInput, calls Filter().
	Otherwise, calls mBufferParentClass::HandleBuffer().
*/
		virtual bool HandleBuffer( BBuffer *inBuffer, FMediaInput *onInput  );

//! FMediaFilter::FormatChanged() -> FilterFormatChanged() wrap
		virtual status_t FormatChanged( FMediaInput *input );
		
//! Creates mOutput, with the same format as mInput, when connected.
		virtual void InputConnected( FMediaInput *input, bool connected = true );

//! Will call FilterFormatChanged().
		virtual void OutputConnected( FMediaOutput *output, bool connected = true );

//! For Control Data buffers.
		typedef FControlDataNode inherited;

	protected:
		media_format mRequiredFormat, mPreferredFormat;
		FMediaInput *mInput;
		FMediaOutput *mOutput;
};

#endif
