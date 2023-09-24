#ifndef F_FTuple_H
#define F_FTuple_H

template <int n, class T>
class FTuple {
	public:
		FTuple( T init = 0 ) {
			value = new T[n];
			for( int i=0; i<n; i++ ) value[i] = init;
		}
		
		FTuple( const FTuple<n,T>& c ) {
			value = new T[n];
			for( int i=0; i<n; i++ ) value[i] = c.value[i];
		}
		
		~FTuple() {
			if( value ) delete value;
		}
		
		FTuple<n,T>& operator =( const FTuple<n,T>& c ) {
			if( value ) delete value;
			value = new T[n];
			for( int i=0; i<n; i++ ) value[i] = c.value[i];
		}

		FTuple<n,T>& operator =( const T c ) {
			for( int i=0; i<n; i++ ) value[i] = c;
		}

		FTuple<n,T>& operator +=( const FTuple<n,T>& c ) {
			for( int i=0; i<n; i++ ) value[i] += c.value[i];
			return( *this );
		}

		FTuple<n,T>& operator -=( const FTuple<n,T>& c ) {
			for( int i=0; i<n; i++ ) value[i] -= c.value[i];
			return( *this );
		}

		FTuple<n,T>& operator *=( const FTuple<n,T>& c ) {
			for( int i=0; i<n; i++ ) value[i] *= c.value[i];
			return( *this );
		}

		FTuple<n,T>& operator *=( const float c ) {
			for( int i=0; i<n; i++ ) value[i] *= c;
			return( *this );
		}

		FTuple<n,T>& operator /=( const FTuple<n,T>& c ) {
			for( int i=0; i<n; i++ ) value[i] /= c.value[i];
			return( *this );
		}
		
		friend FTuple<n,T>& operator +<n,T>( const FTuple<n,T>& a, const FTuple<n,T>& b );
		friend FTuple<n,T>& operator -<n,T>( const FTuple<n,T>& a, const FTuple<n,T>& b );
		friend FTuple<n,T>& operator *<n,T>( const FTuple<n,T>& a, const FTuple<n,T>& b );
		friend FTuple<n,T>& operator /<n,T>( const FTuple<n,T>& a, const FTuple<n,T>& b );

		void Set( int i, T v ) { value[i] = v; }
		T& Get( int i ) { return( value[i] ); }
		T* Pointer() { return( value ); }
		
	protected:
		T *value;
};

template <int n, class T>
inline FTuple<n,T>& operator +( const FTuple<n,T>& a, const FTuple<n,T>& b ) {
	FTuple<n,T> c( a );
	c += b;
	return( c );
}

template <int n, class T>
inline FTuple<n,T>& operator -( const FTuple<n,T>& a, const FTuple<n,T>& b ) {
	FTuple<n,T> c( a );
	c -= b;
	return( c );
}

template <int n, class T>
inline FTuple<n,T>& operator *( const FTuple<n,T>& a, const FTuple<n,T>& b ) {
	FTuple<n,T> c( a );
	c *= b;
	return( c );
}

template <int n, class T>
inline FTuple<n,T>& operator /( const FTuple<n,T>& a, const FTuple<n,T>& b ) {
	FTuple<n,T> c( a );
	c /= b;
	return( c );
}

#endif
