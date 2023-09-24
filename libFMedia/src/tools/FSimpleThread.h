#ifndef F_FSimpleThread_H
#define F_FSimpleThread_H

#include "Locker.h"

class FSimpleThread : public BLocker {
	public:
		FSimpleThread( const char *name = "Some SimpleThread" );
		~FSimpleThread();
	
		void Start( uint32 Priority = B_NORMAL_PRIORITY );
		void Stop();

		virtual void ThreadFunction() = 0;
			/*  Thread function is called when thread is started,
				should contain a
					while( !stStop ) { ...; snooze(...); }
				loop.
			*/

		
	protected:
		const char *stName;
		bool stStop;
		thread_id stThread;
		static int32 stCaller( void *data );
};

#endif
