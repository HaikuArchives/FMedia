#include "FSimpleThread.h"

#include <string.h>
#include <stdio.h>

FSimpleThread::FSimpleThread( const char *name )
	: BLocker( name ) {
	stName = strdup( name );
	stStop = true;
	stThread = 0;
}

FSimpleThread::~FSimpleThread() {
	stStop = true;
	delete stName;
}

void FSimpleThread::Start( uint32 Priority ) {
	stStop = false;
	stThread = spawn_thread( stCaller, stName, Priority, this );
	resume_thread( stThread );
}

void FSimpleThread::Stop() {
	Lock();
	stStop = true;
	Unlock();
	
	status_t ret;
	wait_for_thread( stThread, &ret );
}

int32 FSimpleThread::stCaller( void *data ) {
	FSimpleThread* myThread = (FSimpleThread*)data;
	myThread->ThreadFunction();
	return 0;
}
