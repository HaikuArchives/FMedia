#include "AddOnInstallApp.h"
#include "esoteric.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <FindDirectory.h>
#include <Alert.h>
#include <Path.h>
#include <Entry.h>
#include <Roster.h>
#include <Application.h>

static void
ErrorAlert(const char * message, status_t err)
{
	char msg[256];
	sprintf(msg, "%s\n%s [%x]", message, strerror(err), err);
	(new BAlert("", msg, "Quit"))->Go();
	be_app->PostMessage(B_QUIT_REQUESTED);
}

int main() {
	BApplication app(APP_SIGNATURE);

	int32 Choice = (new BAlert( ADDON_NAME, ADDON_NAME" is a MediaKit AddOn\n\nDo you want to install "ADDON_NAME" to /boot/home/config/add-ons/FMedia now?", "Yes, Copy", "Yes, Link", "No" ))->Go();

	if( Choice == 0 || Choice == 1 ) {

		BPath path;
		if( B_OK != find_directory( B_USER_ADDONS_DIRECTORY, &path, true ) ) {
			path.SetTo("/boot/home/config/addons/");
		}
		path.Append("media/FMedia");

		BEntry entry( path.Path() );
		if( !entry.Exists() ) {
			const char *cmd = "mkdir -p";
			char *Command = new char[ strlen( path.Path() ) + strlen( cmd ) + 4 ];
			sprintf( Command, "%s %s", cmd, path.Path() );
			if( system( Command ) ) {
				(new BAlert( ADDON_NAME, ADDON_NAME": Cannot create add-on directory [B_USER_ADDONS_DIRECTORY]/media/FMedia.\n", "Ah well..."))->Go();
				fprintf( stderr, "Could not create add-on directory %s.\n", path.Path() );
			}
		}

		if( entry.Exists() ) {
			app_info info;
			if( be_app->GetAppInfo( &info ) != B_OK ) exit(-1);

			BPath target( path );
			target.Append(ADDON_NAME);
			BEntry tEntry( target.Path() );
			printf("Looking if %s exists\n", target.Path());
			if( tEntry.Exists() ) {
				printf("...yes. Moving to trash.\n");
				// overwrite
				char *Command = new char[ strlen( target.Path() ) + 64 ];
				sprintf( Command, "mv %s /boot/home/Desktop/Trash/", target.Path() );
				printf( "Trashing: %s\n", Command );
				system( Command );					

				delete Command;
			}
			
			BEntry oentry( &(info.ref) );
			BPath opath;
			if( oentry.GetPath( &opath ) == B_OK ) {

				const char *cmd = Choice==1?"ln -s":"cp -p ";
				char *Command = new char[ strlen( path.Path() ) + strlen( opath.Path() ) + strlen( cmd ) + 4 ];
				sprintf( Command, "%s %s %s", cmd, opath.Path(), path.Path() );
				if( system( Command ) ) {
					(new BAlert( ADDON_NAME, ADDON_NAME": Installation failed for some reason, sorry.\n", "Hm, ok." ))->Go();
					fprintf(stderr, "Could not %s from %s to %s.\n", cmd, opath.Path(), path.Path());
				}
				delete Command;
			}
		}
	}
	be_app->PostMessage(B_QUIT_REQUESTED);
	return(0); 
}


