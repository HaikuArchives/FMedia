#ifndef F_MEDIA_ADDON_H
#define F_MEDIA_ADDON_H

#include <MediaAddOn.h>
#include "esoteric.h"

#include "FDifference.h"
typedef ADDON_CLASS_NAME FMediaAddonClass;

class FMediaAddOn : public BMediaAddOn {
	public:
		FMediaAddOn( image_id mImage );
		
		virtual status_t InitCheck( const char **outFailureText );
		virtual status_t AutoStart( int index, BMediaNode **outNode, int32 *outInternalId, bool *outHasMore );
		virtual bool WantsAutoStart( void );
		
		virtual int32 CountFlavors( void );
		virtual status_t GetFlavorAt( int32 flavorNum, const flavor_info **outInfo );
		
		virtual status_t GetConfigurationFor( BMediaNode *node, BMessage *config );		
		virtual BMediaNode *InstantiateNodeFor( const flavor_info *info, BMessage *config, status_t *outError );
	protected:
};

#endif
