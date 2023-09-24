#ifndef F_FIterator_H
#define F_FIterator_H

#include "FContainer.h"

template <class T>
class FIterator {
	public:
		FIterator( const FContainer<T> *container, const bool backwards = false, const int32 startCookie = -1 ) {
				mContainer = container;
				mBackwards = backwards;
				mCookie = startCookie;
				if( mCookie == -1 ) mCookie = mContainer->Cookie( mBackwards );
			}
			
		FIterator(const FIterator<T> & rhs) {
				*this = rhs;
			}
			
		FIterator<T> &operator=( const FIterator<T> &rhs ) {
				mContainer = rhs.mContainer;
				mCookie = rhs.mCookie;
				mBackwards = rhs.mBackwards;
			}
			
		bool HasMore() const {
				return( Peek() != NULL );
			}
			
		const T* GetNext() {
				mCookie = mContainer->Iterate( mCookie, mBackwards );
				return( mContainer->GetValueFromCookie( mCookie ) );
			}

		const T* Peek() const {
				int32 mPeekCookie = mContainer->Iterate( mCookie, mBackwards );
				return( mContainer->GetValueFromCookie( mPeekCookie ) );
			}

	private:
		const FContainer<T> *mContainer;
		int32 mCookie;
		bool mBackwards;
};

#endif
