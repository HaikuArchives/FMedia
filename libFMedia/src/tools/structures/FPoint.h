#ifndef F_FPoint_H
#define F_FPoint_H

#include <Point.h>

class FPoint : public BPoint {
	public:
		FPoint() : BPoint() { ; }
		FPoint( float X, float Y ) : BPoint( X, Y ) { ; }
		FPoint( const BPoint& pt ) : BPoint( pt ) { ; }
		FPoint( float V ) : BPoint( V, V ) { ; }
		
		BPoint& operator=( const float v ) {
				Set( v, v );
				return( *this );
			}

		BPoint operator /( const float v ) {
				return( BPoint( x/v, y/v ) );
			}
};

#endif
