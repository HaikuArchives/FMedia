#include "FMediaAddOn.h"
#include <string.h>
#include <Message.h>

extern "C" _EXPORT BMediaAddOn *make_media_addon( image_id mImage ) {
	return( new FMediaAddOn( mImage ) );
}

FMediaAddOn::FMediaAddOn( image_id mImage )
	: BMediaAddOn( mImage ) {
}
		
status_t FMediaAddOn::InitCheck( const char **outFailureText ) {
	*outFailureText = "No error I would know of, sorry.";
	return B_OK;
}

status_t FMediaAddOn::AutoStart( int index, BMediaNode **outNode, int32 *outInternalId, bool *outHasMore ) {
	return B_ERROR;
}

bool FMediaAddOn::WantsAutoStart( void ) {
	return( false );
}
		
int32 FMediaAddOn::CountFlavors( void ) {
	return( 1 );
}

status_t FMediaAddOn::GetFlavorAt( int32 n, const flavor_info **out_info ) {
	if (n != 0) { 
		return B_ERROR; 
	} 
	
	flavor_info *flavorInfo = new flavor_info;
	
	flavorInfo->internal_id = n;
	flavorInfo->name = strdup( ADDON_NAME );
	flavorInfo->info = strdup( ADDON_INFO );
	flavorInfo->kinds = B_BUFFER_CONSUMER | B_BUFFER_PRODUCER;
	flavorInfo->flavor_flags = 0;
	flavorInfo->possible_count = 0;
	
	// Set up the list of input formats. We only support raw video input. 
	
	flavorInfo->in_format_count = 1; 
	media_format *aFormat = new media_format; 
	aFormat->type = B_MEDIA_RAW_VIDEO; 
	aFormat->u.raw_video = media_raw_video_format::wildcard; 
	flavorInfo->in_formats = aFormat; 
	
	// same for output
	flavorInfo->out_format_count = 1; 
	flavorInfo->out_formats = aFormat; 
	
	// And set up the result pointer 
	
	*out_info = flavorInfo; 
	return B_OK;
}
		
status_t FMediaAddOn::GetConfigurationFor( BMediaNode *node, BMessage *config ) {
	config->AddString("message", "Node not properly saved, sorry.");
	return B_ERROR;
}

BMediaNode *FMediaAddOn::InstantiateNodeFor( const flavor_info *info, BMessage *config, status_t *outError ) {
	return new FMediaAddonClass( ADDON_NAME );
}

