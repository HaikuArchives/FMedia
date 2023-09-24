#ifndef F_FState_H
#define F_FState_H

#define FState_ERROR //printf
#define FState_INFO //printf

#include "FMedia.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <String.h>
#include <Node.h>
#include <Path.h>
#include <Entry.h>
#include <FindDirectory.h>

template <class T>
class FState {
	public:
		FState( const char *name, type_code type, const char *settingsFile = "settings", const char *settingsDir = F_MEDIA_SETTINGS_PATH )
				: mName( name ),
				  mNode( NULL ) {
				  
				  mType = type;

				// Find Settings File
				BPath path;
				if( B_OK == find_directory( B_USER_SETTINGS_DIRECTORY, &path, true ) ) {
					path.Append( settingsDir );
					BEntry entry( path.Path() );
					if( !entry.Exists() ) {
						const char *cmd = "mkdir -p";
						char *Command = new char[ strlen( path.Path() ) + strlen( cmd ) + 4 ];
						sprintf( Command, "%s %s", cmd, path.Path() );
						if( system( Command ) ) {
							FState_ERROR("FState '%s': Cannot create settings directory %s.\n", mName.String(), path.Path() );
						}
					}
					
					path.Append( settingsFile );
			
					entry.SetTo( path.Path() );
					if( !entry.Exists() ) {
						const char *cmd = "touch ";
						char *Command = new char[ strlen( path.Path() ) + strlen( cmd ) + 4 ];
						sprintf( Command, "%s %s", cmd, path.Path() );
						if( system( Command ) ) {
							FState_ERROR("FState '%s': Cannot create settings file %s.\n", mName.String(), path.Path() );
						}
					}
			
			
					mNode = new BNode( path.Path() );
					status_t err;
					if( (err = mNode->InitCheck()) != B_OK ) {
						FState_ERROR("FState '%s': Cannot save state in %s: %s\n", mName.String(), path.Path(), strerror(err));
						mNode = NULL;
					}
				}
			}

		T Get( T defaultValue ) {
				if( !mNode ) {
					FState_INFO( "FState '%s': No Node\n", mName.String() );
					return( defaultValue );
				}
				
				T ret;
				size_t sz;
				if( (sz = mNode->ReadAttr( mName.String(), mType, 0, &ret, sizeof( T ) ) ) == sizeof( T ) ) {
					FState_INFO("FState '%s': State Read\n", mName.String() );
					return( ret );
				} else {
					FState_ERROR("FState '%s': Could not read state\n", mName.String() );
					return( defaultValue );
				}
			}
		
		status_t Put( T value ) {
				if( !mNode ) return( B_ERROR );
			
				if( mNode->WriteAttr( mName.String(), mType, 0, &value, sizeof( T ) ) == sizeof( T ) ) {
					FState_INFO("FState '%s': State Saved\n", mName.String() );
					return( B_OK );
				} else {
					return( B_ERROR );
				}
			}
		
	protected:
		BString mName;
		type_code mType;
		BNode *mNode;
};

inline status_t FState<const char *>::Put( const char *value ) {
	if( !mNode ) return( B_ERROR );

	if( mNode->WriteAttr( mName.String(), mType, 0, value, strlen( value )+1 ) == strlen( value )+1 ) {
		FState_INFO("FState '%s': State Saved\n", mName.String() );
		return( B_OK );
	} else {
		return( B_ERROR );
	}
}

inline const char * FState<const char *>::Get( const char *defaultValue ) {
	static char * ret = new char[ 0xff ];

	if( !mNode ) {
		FState_INFO( "FState '%s': No Node\n", mName.String() );
		return( defaultValue );
	}
	
	size_t sz = 0xff;
	if( (mNode->ReadAttr( mName.String(), mType, 0, ret, sz ) ) > 0 ) {
		FState_INFO("FState '%s': State Read\n", mName.String() );
		return( ret );
	} else {
		FState_ERROR("FState '%s': Could not read state\n", mName.String() );
		return( defaultValue );
	}
}

#endif