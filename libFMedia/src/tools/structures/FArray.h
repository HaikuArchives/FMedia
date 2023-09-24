/*	
	class f::FArray
 	
 	lightweight (all-inline) FArray in BListish API.
 	
 	API overview:

		class FArray<t> {
			FArray(int _blockSize=64);
			FArray(FArray<t> &copyFrom);
			
			// add/remove
			int AddItem(const t &theT);
			int AddFArray(FArray<t> *a);
			void RemoveItem(int index);
			void RemoveItems(int index, int len);
			void MakeEmpty();
			
			void Trim();
			
			// query
			t* ItemAt(int index) const;
			t* operator[](int index);
			t* Items();
			bool IsMember(t &theT);
			
			// reset array to this list
			void SetList(t* newList, int listSize);
			
			// force array size
			void SetSlots(int slots);
			void SetItems(int count);
		};
		
	history:
		020105 (dan) 
			from some BFArray.h, Be Sample Code License assumed
			(file carried no copyright notice).
*/
/*
	-----------------------------------------------------------------------
	portions Copyright 1991-2001 Be, Inc.
	See the included BeSampleCodeLicense.txt for details.
	-----------------------------------------------------------------------
*/ 

#ifndef F_FArray_H
#define F_FArray_H

#include <memory.h>
#include <malloc.h>

#include "FContainer.h"


template <class t>
class FArray : public FContainer<t> {

	public:
		inline FArray( int _blockSize=64 ) {
				blockSize = _blockSize;
				numItems = numSlots = 0;
				items = NULL;
			};
			
		inline FArray(FArray<t> &copyFrom) {
				mOwnsItems = copyFrom.mOwnsItems; // possibly dangerous.
				blockSize = copyFrom.blockSize;
				numSlots = 0;
				items = NULL;
				AssertSize(copyFrom.numSlots);
				numItems = copyFrom.numItems;
				memcpy(items,copyFrom.items,numItems*sizeof(t));
			};
			
		virtual inline ~FArray() {
				if (items)
					free(items);
			};
			
		inline t* Items() {
				return items;
			};
			
		inline void SetList(t* newList, int listSize) {
				if (items)
					free(items);
				items = newList;
				numSlots = listSize;
				numItems = listSize;
			};
			
		inline void SetSlots(int slots) {
				if (numSlots != slots) {
					numSlots = slots;
					if (numItems > numSlots)
						numItems = numSlots;
					items = (t*)realloc(items,numSlots*sizeof(t));
				};
			};
			
		inline void SetItems(int count)
			{
				AssertSize(count);
				numItems = count;
			};
			
		inline void Trim() {
				SetSlots(numItems);
			};
			
		inline int CountItems() const { return numItems; };
			
		inline	bool IsMember(t &theT) {
				for (int i=0;i<numItems;i++)
					if (theT == items[i]) return true;
				return false;
			};
			
		inline void RemoveItems(int index, int len) {
				memmove(items+index,items+index+len,sizeof(t)*(numItems-index-len));
				numItems-=len;
			};
			
		inline void RemoveItem(t theT) {
				for (int i=0;i<numItems;i++)
					if (theT == items[i]) RemoveItems( i, 1 );
			};

		inline void RemoveItem(int index) {
				RemoveItems(index,1);
			};

		inline int IndexOf(t theT) {
				for (int i=0;i<numItems;i++)
					if (theT == items[i]) return( i );
				return -1;
			};
			
		inline int AddFArray(FArray<t> *a) {
				AssertSize(numItems + a->numItems);
				memcpy(items+numItems,a->items,a->numItems*sizeof(t));
				numItems = numItems + a->numItems;
				return a->numItems;
			};
			
		inline int AddItem(const t &theT) {
				AssertSize(numItems+1);
				items[numItems] = theT;
				numItems++;
				return numItems-1;
			};
			
		inline void MakeEmpty() {
				numSlots = 0;
				numItems = 0;
				if (items!=NULL) {
					delete items;
					items = NULL;
				};
			};
		
		inline t* ItemAt(int index) const
			{ return( ( index<0 || index>=numItems ) ? NULL : &items[index] ); };
		
		inline t* operator[](int index)
			{ return &items[index]; };


		// IteratorSupport (FContainer)
		virtual t* GetValueFromCookie( const int32 cookie ) const {
					return ItemAt( cookie );
				}
		virtual int32 Iterate( int32 cookie, const bool backwards ) const {
					return backwards ? cookie-1 : cookie+1;
				}
		virtual int32 Cookie( const bool backwards = false ) const {
					return backwards ? CountItems()+1 : -1;
				}

		// Size()
		virtual int32 Size() const {
					return CountItems();
				}
				
		virtual void Clear() {
					numItems = 0;
				}
		
	protected:

		t		*items;
		int		numItems;
		int		numSlots;
		int		blockSize;

		inline void AssertSize(int size) {
				if (size > numSlots) {
					numSlots = ((size+blockSize-1)/blockSize) * blockSize;
					items = new t[numSlots]; //(t*)realloc(items,numSlots*sizeof(t));
				};
			};

};

#endif
