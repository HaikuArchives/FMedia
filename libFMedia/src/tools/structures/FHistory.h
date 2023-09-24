#ifndef F_FHistoryr_H
#define F_FHistoryr_H


template <class T>
class FHistory {
	public:
		FHistory( int32 nvalues = 100, T init = 0 ) {
			nValues = nvalues;
			cValue = 0;
			Values = new T[ nValues ];
			for( int i=0; i<nValues; i++ ) {
				Values[i] = init;
			}
		}
		
		~FHistory() {
			delete Values;
		}
		
		void Add( T value ) {
			cValue++;
			if( cValue >= nValues ) cValue = 0;
			Values[cValue] = value;
		}
		
		T Get( int32 time ) const {
			int32 i = cValue - time;
			while( i < 0 ) i+=nValues;
			return( Values[i] );
		}
		
		int32 CountValues() { return nValues; }
	
	protected:
		T *Values;
		int32 nValues;
		int32 cValue;
};


#endif
