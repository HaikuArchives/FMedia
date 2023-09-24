#ifndef F_FStrongest_H
#define F_FStrongest_H

#include "FArray.h"

#include <stdio.h>
#define INFO printf

#define F_STRENGTH_VARIANCE 50

template <class T>
class FStrengthValue {
	public:
		FStrengthValue( T value, T variance = F_STRENGTH_VARIANCE ) {
				mValue = value;
				mVariance = variance;	
				mTop = mValue+mVariance;
				mBottom = mValue > mVariance ? mValue - mVariance : mValue;
				mStrength = 0;
			}
		
		bool operator ==( T c ) {
				return( c >= mBottom && c <= mTop );
			}
			
		bool operator >( FStrengthValue<T>& c ) {
				if( mStrength > 0 ) {
					INFO("Stronger? %i > %i?\n", mStrength, c.mStrength );
				}
				return( mStrength > c.mStrength );
			}
			
		FStrengthValue<T>& operator ++() {
				mStrength++;
				return( *this );
			}
			
		T Value() {
				return mValue;
			}
		
	protected:
		T mValue;
		T mVariance;
		T mBottom, mTop;
		
		float mStrength;
};


template <class T>
class FStrongest {
	public:
		FStrongest() {
				mStrongest = NULL;
			}
			
		FStrongest<T>& operator +=( T value ) {
				FStrengthValue<T> *found = FindItem( value );
				if( found ) {
					++(*found);
					printf("ADD");
				} else {
					found = new FStrengthValue<T>( value );
					if( found ) {
						found++;
						mValues.AddItem( found );
					//	printf("n");
					}
				}
				fflush( stdout );
				Compare( found );
			}
			
		FStrengthValue<T>* FindItem( T value ) {
				for( int i=0; i<mValues.CountItems(); i++ ) {
					if( **(mValues.ItemAt( i )) == value ) return *(mValues.ItemAt(i));
				}
				return NULL;
			}
			
		void Compare( FStrengthValue<T> *to ) {
				if( !mStrongest ) {
					mStrongest = to;
					return;
				}
				INFO("Compare %p to %p\n", to, mStrongest );
				if( (*to) > (*mStrongest) ) mStrongest = to;
			}
		
		T Value() {
				return (mStrongest!=NULL) ? mStrongest->Value() : 0x88;
			}
	protected:
		FArray< FStrengthValue<T>* > mValues;
		FStrengthValue<T> *mStrongest;
};

#endif
