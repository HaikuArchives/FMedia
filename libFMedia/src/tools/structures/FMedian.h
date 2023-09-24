#ifndef F_FMedian_H
#define F_FMedian_H

template <class T>
class FMedianValue {
	public:
		FMedianValue( T value );
		
	protected:
		T mValue;
		FMedianValue<T> *next, *previous;
};

template <class T>
class FMedian {
	public:
		FMedian( int32 nvalues = 3, T init = 0 ) {
			nValues = nvalues;
			cValue = 0;
			Values = new T[ nValues ];
			for( int i=0; i<nValues; i++ ) {
				Values[i] = init;
			}
		}
		
		~FMedian() {
			delete Values;
		}
		
		void Add( T value ) {
			cValue++;
			if( cValue >= nValues ) cValue = 0;
			Values[cValue] = value;
		}
		
		T Average() const {
			T acc = 0;
			for( int i=0; i<nValues; i++ ) {
				acc += Values[i];
			}
			return( acc/nValues );
		}

		T Max() const {
			T a = 0;
			for( int i=0; i<nValues; i++ ) {
				if( Values[i] > a ) a = Values[i];
			}
			return( a );
		}

		T Min() const {
			T a = 1000;		// 2do: bullshit!
			for( int i=0; i<nValues; i++ ) {
				if( Values[i] < a ) a = Values[i];
			}
			return( a );
		}
	
	protected:
		T *Values;
		int32 nValues;
		int32 cValue;
};


#endif
