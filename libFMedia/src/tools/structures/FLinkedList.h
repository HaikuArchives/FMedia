#ifndef F_FLinkedList_H
#define F_FLinkedList_H

/*
	non-threadsafe Doubly Linked List.
*/

/*
	LinkedListItem
*/

#include <stdio.h>
#define WARNING printf

template <class T>
class FLinkedList;

template <class T>
class FLinkedListItem {
	public:
		FLinkedListItem( T *data, FLinkedListItem* insertAfter = NULL ) {
				mData = data;
				mPrevious = insertAfter;
				mNext = insertAfter ? insertAfter->ReplaceNext( this ) : NULL;
				if( mNext ) mNext->mPrevious = this;
			}
		
		FLinkedListItem<T> *ReplaceNext( FLinkedListItem<T> *next ) {
				FLinkedListItem<T> *ret = mNext;
				mNext = next;
				return ret;
			}
			
		void Remove( FLinkedList<T> *list ) {
				if( !mPrevious ) list->mFirst = mNext;
				else mPrevious->mNext = mNext;
				if( !mNext ) list->mLast = mPrevious;
				else mNext->mPrevious = mPrevious;
			}
			
		FLinkedListItem<T> *Next() {
				return( mNext );
			}

		FLinkedListItem<T> *Previous() {
				return( mPrevious );
			}

		T* Data() {
				return( mData );
			}
			
	protected:
		FLinkedListItem<T> *mPrevious;
		FLinkedListItem<T> *mNext;
		T *mData;
};


/*
	LinkedList
*/

template <class T>
class FLinkedList {
	public:
		FLinkedList( bool ownership = true )
				: mOwnership( ownership ) {
				mFirst = mLast = NULL;
			}

		~FLinkedList() {
				FLinkedListItem<T> *item = mFirst, *next;
				if( mOwnership ) {
					while( item ) {
						if( item->Data() ) delete item->Data();
						next = item->Next();
						delete item;
						item = next;
					}
				} else {
					while( item ) {
						next = item->Next();
						delete item;
						item = next;
					}
				}
			}
			
		void Add( T *data ) {
				if( !mLast ) {
					if( !mFirst ) {
						mFirst = mLast = new FLinkedListItem<T>( data, NULL );
					} else {
						WARNING("Linked List has First but no Last item\n");
					}
				} else {
					mLast = new FLinkedListItem<T>( data, mLast );
				}
			}

		FLinkedListItem<T> *First() { return mFirst; };
		FLinkedListItem<T> *Last() { return mLast; };

		friend class FLinkedListItem<T>;
			
	protected:
		FLinkedListItem<T> *mFirst;
		FLinkedListItem<T> *mLast;
		bool mOwnership;
};

#endif
