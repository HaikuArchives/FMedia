#ifndef F_FContainer_H
#define F_FContainer_H


// Container

template <class t>
class FContainer {
	public:
		// Iterator support
		virtual t* GetValueFromCookie( const int32 cookie ) const = 0;
		virtual int32 Iterate( int32 cookie, const bool backwards ) const = 0;
		virtual int32 Cookie( const bool backwards = false ) const = 0;

		// other
		virtual int32 Size() const = 0;
		virtual void Clear() = 0;
};


#endif
