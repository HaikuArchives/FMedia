#include "FControllable.h"

#include <TimedEventQueue.h>
#include <ParameterWeb.h>
#include <TimeSource.h>

#include "FIterator.h"

#include <ClassInfo.h>
#include <stdio.h>
#include <string.h>


#define CALL //printf
#define INFO //printf
#define ERROR printf
#define WARNING printf
#define FORMAT //printf

FControllable::FControllable( const char *name, media_type in_type, media_type out_type, BMediaAddOn *addon )
	: BControllable(), FMediaNode( name, in_type, out_type, addon ), BMediaNode( name ) {
}

FControllable::~FControllable() {
}

status_t FControllable::HandleMessage( int32 code, const void *data, size_t sz ) {
	if( BControllable::HandleMessage( code, data, sz )
		&& FMediaNode::HandleMessage( code, data, sz ) )
		HandleBadMessage( code, data, sz );
		
	return B_ERROR;
}

status_t FControllable::GetParameterValue( int32 id, bigtime_t *last_change, void *value, size_t* ioSize ) {
	CALL("FControllable::GetParameterValue\n");

		FParameter *Param = FindParameter( id );
		if( Param ) {
			*last_change = TimeSource()->Now();
			return( Param->Get( value, ioSize ) );
		}
		
	return B_ERROR;
}

void FControllable::SetParameterValue( int32 id, bigtime_t performance_time, const void *value, size_t ioSize ) {
	CALL("FControllable::SetParameterValue(%i)\n", id);

		FParameter *Param = FindParameter( id );
		if( Param ) {
			Param->Set( value, ioSize );
			ParameterChanged( Param );
			
			BroadcastNewParameterValue( performance_time, id, (void*)value, ioSize ); // !casting away const
			
			return;
		} else
			WARNING("No Parameter #%i\n", id );
}

int32 FControllable::AddParameter( FParameter *Parameter ) {
	return mParameters.AddItem( Parameter );
}

FParameter *FControllable::FindParameter( int32 id ) {
	FParameter **p = mParameters.ItemAt( id );
	return p!=NULL ? *p : NULL;
}

void FControllable::ClearParameters() {
	SetParameterWeb( NULL );
	mParameters.Clear();
	//! /todo delete what you create!
}

void FControllable::ParameterChanged( FParameter *parameter ) {
	// generic is noop
}

BParameterWeb* FControllable::BuildParameterWeb() {
	CALL("FControllable::BuildParameterWeb\n");

	BParameterWeb *web = new BParameterWeb;
	BParameterGroup *maingroup = web->MakeGroup("FParameters");
	BParameterGroup *group = maingroup->MakeGroup(Name());

	FIterator<FParameter*> params( &mParameters );
	FParameter *param;
	int i=0;
	while( params.HasMore() && NULL != (param = *params.GetNext()) ) {
		if( is_kind_of( param, FParameterGroup ) ) {
			group = param->CreateInGroup( maingroup, i );
		} else {
			group = param->CreateInGroup( group, i );
		}
		param->SetID(i);
		i++;
	}
	return web;	
}

void FControllable::MakeParameterWeb() {
	SetParameterWeb( BuildParameterWeb() );
}

void FControllable::Setup() {
	CALL("FControllable::Setup\n");
	MakeParameterWeb();
};
