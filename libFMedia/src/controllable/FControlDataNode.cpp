#include "FControlDataNode.h"
#include "FMediaFormats.h"
#include "FIterator.h"

#include <stdio.h>
#define INFO //printf

FControlDataNode::FControlDataNode( const char *name, media_type in_type, media_type out_type, BMediaAddOn *addon )
	: FControllable( name, in_type, out_type, addon )
	, BMediaNode( name )
	{
	
}

FControlDataNode::~FControlDataNode() {
	//! /todo should dump mControlDataParameters
}

void FControlDataNode::Setup() {
	inherited::Setup();
}

int32 FControlDataNode::AddControlDataParameter( FParameter *param, int mode ) {
	FControlDataParameter *cdataParam = new FControlDataParameter( param );
	
	
	char name[ F_PARAMETER_NAME_LENGTH ];
	snprintf( name, F_PARAMETER_NAME_LENGTH, "%s(%i).%s", Name(), ID(), param->Name() );	
	media_format cdataFormat = FGetControlDataFormat( name );	
		
	if( mode & F_CONTROLDATA_IN ) {
		cdataParam->Input = new FMediaInput( this, param->Name(), cdataFormat, cdataFormat );
		AddInput( cdataParam->Input );
		INFO("Added input %s\n", param->Name() );
	}
	if( mode & F_CONTROLDATA_OUT ) {
		cdataParam->Output = new FMediaOutput( this, param->Name(), cdataFormat, cdataFormat, F_SIMPLE_BUFFERS, new FBufferManager( param->Name() )  );
		AddOutput( cdataParam->Output );
		INFO("Added output %s\n", param->Name() );
	}
	mControlDataParameters.AddItem( cdataParam );
	
	return( AddParameter( param ) );
}

void FControlDataNode::InputConnected( FMediaInput *input, bool connected ) {
	FIterator<FControlDataParameter*> params( &mControlDataParameters );
	FControlDataParameter *param;
	while( params.HasMore() && NULL != (param = *params.GetNext()) ) {
		if( param->Input == input && input->Format().MetaDataSize() == sizeof( FControlDataFormat )) {
			FControlDataFormat *cdFormat = (FControlDataFormat *)input->Format().MetaData();
			input->SetName( cdFormat->Name );
			param->Parameter->SetName( cdFormat->Name );
			INFO("New Name: %s/%s\n", cdFormat->Name, input->Name() );
			if( param->Output ) param->Output->SetName( cdFormat->Name );
			Setup();
		}
	}
}

bool FControlDataNode::HandleBuffer( BBuffer *inBuffer, FMediaInput *onInput  ) {
	bool handled = false;
	FIterator<FControlDataParameter*> params( &mControlDataParameters );
	FControlDataParameter *param;
	while( params.HasMore() && NULL != (param = *params.GetNext()) ) {
		if( param->Input == onInput ) {
			INFO("%s: Parameter %s changed to %.2f (sz %i)...\n", Name(), param->Parameter->Name(), *((float*)inBuffer->Data()), inBuffer->SizeUsed());
/*			param->Parameter->Set( inBuffer->Data(), inBuffer->SizeUsed() );
			BroadcastNewParameterValue( inBuffer->Header()->start_time,
										param->Parameter->GetID(),
										inBuffer->Data(),
										inBuffer->SizeUsed() ); 
			ParameterChanged( param->Parameter );
*/
			SetParameterValue( param->Parameter->GetID(), inBuffer->Header()->start_time, inBuffer->Data(), inBuffer->SizeUsed() );
			handled = true;
		}
	}
	if( !handled ) return inherited::HandleBuffer( inBuffer, onInput );
	else 
		RecycleBuffer( inBuffer );
}

void FControlDataNode::ParameterChanged( FParameter *onParam ) {
	FIterator<FControlDataParameter*> params( &mControlDataParameters );
	FControlDataParameter *param;
	while( params.HasMore() && NULL != (param = *params.GetNext()) ) {
		if( param->Parameter == onParam && param->Output && param->Output->IsConnected() ) {
			INFO("%s: Sending control data change...\n", Name());
			BBuffer *buffer = param->Output->GetBufferManager()->GetBuffer( sizeof( float ) );
			if( buffer ) {
				size_t sz = buffer->SizeAvailable();
				param->Parameter->Get( buffer->Data(), &sz );
				buffer->Header()->size_used = sz;
				status_t err = SendOrRecycleBuffer( buffer, param->Output );
//				INFO("\t buffer sent: %s\n", strerror(err));
			}
		}
	}
}