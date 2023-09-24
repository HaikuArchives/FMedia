#ifndef F_FControllable_H
#define F_FControllable_H

#include <List.h>
#include <Controllable.h>
#include "FMediaNode.h"
#include "FParameter.h"

//!	a FMediaNodes with functionality to handle Parameters
/*	
	derive from FControllable when your node has parameters (settings). Create and add your parameters
	alongside your in-/outputs in your override to Setup(), and make sure you call inherited Setup().
	
	if you need complex grouping in your parameter web, override BuildParameterWeb.
	not sure how to handle IDs with this.
	
	override ParameterChanged( int32 id ) to react on changes,
	otherwise just use FParameter->Get() in your Filter function.
		
	should implement Be Control Data media type some day.
*/
class FControllable : public BControllable, public FMediaNode {
	public:
		FControllable( const char *name, media_type in_type, media_type out_type, BMediaAddOn *addon );
		~FControllable();
		
		virtual status_t HandleMessage( int32, const void *, size_t );

	protected:
//! hook: Add your FParameter-derived objects here. 
/*!
	make sure you call the Setup() functions of your direct parents, as this function is used
	to add inputs, outputs and parameters, as well as additional stuff.
	Call FControllable's Setup() after you have added your parameters, as it will call MakeParameterWeb()
*/
		virtual void Setup();

//! hook that can be implemented to react on parameter changes.
/*!
	usually it is okay to just query the parameter's value in your filter function,
	but it might be necessary to do some bigger adjustment when the user (or whoever) changes
	a specific parameter. If so, override this function.
	
	This implementation does nothing.
*/
		virtual void ParameterChanged( FParameter *parameter );

//! Add a parameter. Do not delete it after you passed it here.
		int32 AddParameter( FParameter *Parameter );

//! Find a parameter by ID.
		FParameter *FindParameter( int32 id );

//! Clear my list of parameters.
		void ClearParameters();

//! (re-)create and publish the BParameterWeb from my list of parameters
/*!
	This is called from Setup(). You might want to call it whenever you change the list
	of parameters. 
	\todo (I'm not sure if thats really necessary right now.) 
*/
		void MakeParameterWeb();

	protected:		
		// BControllable
		virtual void SetParameterValue( int32 id, bigtime_t performance_time, const void *value, size_t ioSize );
		virtual status_t GetParameterValue( int32 id, bigtime_t *last_change, void *value, size_t* ioSize );

		virtual BParameterWeb *BuildParameterWeb();

	private:
		FArray<FParameter *> mParameters;
};

#endif
