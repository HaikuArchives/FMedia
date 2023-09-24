/*
	classes f::FHashtable and f::FHashtableIterator
		
*/
/*
	-----------------------------------------------------------------------
	portions Copyright 2000 Level Control Systems.
	-----------------------------------------------------------------------
	portions Copyright 1997 Michael Olivero 
	See the included licenses/Olivero.txt for details.
	-----------------------------------------------------------------------
*/ 
/*
   This class is a based on the Java FHashtable class and as such contains all the public
   member functions of its Java equivalent.  Unlike Java, typecasts are not necessary
   since C++ allows template instatiation of types at compile time.

   Note:  Since java has hashCode() as a member of the base Object class, all 
   Java classes are inherently hashable.  Since the template parameter types do
   not necessarily have to have a built in hashing function, the user of the class
   must specify a hash function by calling setHashFunction() passing a pointer 
   to the hash function.

   The has function must be declared as the following:

       uint32 function(const KeyType&);

   Where:
   function = any name you choose to use for the function name
   KeyType  = the type used for the key in the construction of the FHashtable object.

   Example:
       uint32 myHash( const int&)
       {
           //your hashing code here for a key of type int.
       }
*/

#ifndef F_FHashtable_H
#define F_FHashtable_H

#include "FDictionary.h"

#define WARN_OUT_OF_MEMORY printf("ERROR--OUT OF MEMORY!  (%s:%i)\n",__FILE__,__LINE__)

#ifndef newnothrow
#define newnothrow new (nothrow) 
#endif

template <class KeyType, class ValueType> class FHashtable;  // forward declaration

/**
 * This class is an iterator object, used for iterating over the set
 * of keys or values in a FHashtable.  Note that the FHashtable
 * maintains the ordering of keys and values, unlike many hash table
 * implementations.
 *
 * Given a FHashtable object, you can obtain one or more of these
 * iterator objects by calling the FHashtable's GetIterator() method.
 *
 * This iterator actually contains separate state for two iterations:
 * one for iterating over the values in the FHashtable, and one for 
 * iterating over the keys.  These two iterations can be done independently
 * of each other.
 *
 * As of MUSCLE v1.92, it is safe to modify the hashtable during a traversal.
 */
template <class KeyType, class ValueType> class FHashtableIterator : public FIterator<ValueType>
{
public:
   /**
    * Default constructor.  It's here only so that you can include PortableFHashtableIterators
    * as member variables, in arrays, etc.  PortableFHashtableIterators created with this
    * constructor are "empty", so they won't be useful until you set them equal to a 
    * FHashtableIterator that was returned by FHashtable::GetIterator().
    */
   FHashtableIterator();

   /** Copy Constructor.  */
   FHashtableIterator(const FHashtableIterator<KeyType, ValueType> & rhs);

   /** Destructor */
   virtual ~FHashtableIterator();

   /** Assignment operator. */
   FHashtableIterator<KeyType,ValueType> & operator=(const FHashtableIterator<KeyType,ValueType> & rhs);

   /** Returns true iff there are more keys left in the key traversal.  */
   bool HasMoreKeys() const;

   /**
    * Gets the next key in the key traversal.
    * @param setNextKey On success, the next key is copied into this object.
    * @return True on success, false if there are no more keys left.
    */
   bool GetNextKey(KeyType & setNextKey);

   /**
    * Gets a pointer to the next key in the key traversal.
    * Note that the returned pointer is only guaranteed valid as long as the FHashtable remains unchanged.
    * @return A pointer to the next key in the key traversal, or NULL if there are no more keys left.
    */
   KeyType * GetNextKey();

   /**
    * Peek at the next key without modifying the state of the traversal.
    * @param setKey On success, the next key is copied into this object.
    * @return True on success, or false if there are no more keys left.
    */
   bool PeekNextKey(KeyType & setKey) const;

   /**
    * Peek at the next key without modifying the state of the traversal.
    * Note that the returned pointer is only guaranteed valid as long as the FHashtable remains unchanged.
    * @return a pointer to the next key in the key traversal, or NULL if there are no keys left.
    */
   KeyType * PeekNextKey() const;

   /** Returns true iff there are more values left in the value traversal.  */
   virtual bool HasMoreValues() const;

   /**
    * Get the next value in the values traversal.
    * @param setNextValue On success, the next value in the traversal is copied into this object.
    * @return True on success, false if there are no more values left in the value traversal.
    */
   bool GetNextValue(ValueType & setNextValue);

   /**
    * Get the next value in the values traversal.
    * Note that the returned pointer is only guaranteed valid as long as the FHashtable remains unchanged.
    * @return a pointer to the next value in the value traversal, or NULL if there are no values left.
    */
   virtual ValueType * GetNextValue();

   /**
    * Peek at the next value without modifying the state of the traversal.
    * @param setValue On success, the next value is copied into this object.
    * @return True on success, or false if there are no more values left.
    */
   bool PeekNextValue(ValueType & setValue) const;

   /**
    * Peek at the next value without modifying the state of the traversal.
    * Note that the returned pointer is only guaranteed valid as long as the FHashtable remains unchanged.
    * @return a pointer to the next value in the value traversal, or NULL if there are no values left.
    */
   ValueType * PeekNextValue() const;

private:
   friend class FHashtable<KeyType, ValueType>;

   FHashtableIterator(const FHashtable<KeyType, ValueType> & owner, void * startCookie, bool backwards);

   void * _scratchSpace[2];   // ignore this; it's temp scratch space used by GrowTable().

   void * _nextKeyCookie;
   void * _nextValueCookie;
   bool _backwards;

   FHashtableIterator<KeyType, ValueType> * _prevIter;  // for the doubly linked list so that the table can notify us if it is modified
   FHashtableIterator<KeyType, ValueType> * _nextIter;  // for the doubly linked list so that the table can notify us if it is modified

   const FHashtable<KeyType, ValueType>   * _owner;     // table that we are associated with
};


/**
 *  This is a handy templated FHashtable class, rather similar to Java's java.util.FHashtable.
 */
template <class KeyType, class ValueType> class FHashtable : public FDictionary<KeyType,ValueType>
{
public:
   /** This is the signature of the type of callback function that you can pass into the
     * SetCompareFunction() and Sort() methods.  The function should work like strcmp(),
     * return a negative value if (key1) is less than (key2), 0 if the two keys are equal,
     * or a positive value if (key1) is greater than (key2).
     * @param key1 The first key.
     * @param key2 The second key.
     * @param cookie A user-defined value that was passed in to the Sort() or SetCompareFunction() method.
     * @return A value indicating which key is "larger", as defined above.
     */
   typedef int (*KeyCompareFunc)(const KeyType& key1, const KeyType& key2, void * cookie);

   /** This is the signature of the type of callback function that you must pass into the
     * FHashtable constructor or the SetHashFunction method.  The hashtable will use
     * this function to generate hash codes for the key objects you give it, as necessary.
     * @param key The key object to find a hash code for.
     * @return The hash code for that key object.
     */ 
   typedef uint32 (*KeyHashFunc)(const KeyType& key);

   /** Constructor.
    *  @param func Specified a hashing function that returns an integer hashcode for a given key object.
    *  @param initialCapacity Specifies the number of table slots to initially pre-allocate.   Defaults to 13.
    *  @param loadFactor Specifies how densely populated the table must be before it is re-hashed.  Defaults to 0.5 (i.e. 50%)
    */
   FHashtable(KeyHashFunc func, uint32 initialCapacity = 13, float loadFactor = 0.5f);

   /** Copy Constructor.  */
   FHashtable(const FHashtable<KeyType,ValueType> & rhs);

   /** Assignment operator. */
   FHashtable<KeyType,ValueType> & operator=(const FHashtable<KeyType,ValueType> & rhs);

   /** Destructor.  */
   ~FHashtable();

   /** Returns the number of items in the table. */
   virtual uint32 Size() const {return _count;}

   /** Returns true iff the table is empty. */
   virtual bool IsEmpty() const {return _count == 0;}

   /** Returns true iff the table contains a mapping with the given value.  (O(n) search time) */
   bool ContainsValue(const ValueType& value) const;

   /** Returns true iff the table contains a mapping with the given key.  (O(1) search time) */
   bool ContainsKey( const KeyType& key) const;

   /** Retrieve the associated value from the table for a given key.
    *  @param key The key to use to look up a value.
    *  @param setValue On success, the associated value is copied into this object.
    *  @return True on success, false if their was no value found for the given key.
    */
   bool Get(const KeyType& key, ValueType& setValue) const; 

   /** Retrieve a pointer to the associated value object for the given key.
    *  @param key The key to use to look up a value.
    *  @return A pointer to the internally held value object for the given key,
    *          or NULL of no object was found.  Note that this object is only
    *          guaranteed to remain valid as long as the FHashtable remains unchanged.
    */
   virtual ValueType * Get(const KeyType & key) const;

   /** Get an iterator for use with this table.
     * @param backwards Set this to true if you want to iterate through the item list backwards.  Defaults to false.
     * @return an iterator object that can be used to examine the items in the hash table. 
     */
   FIterator<ValueType> *CreateIterator(bool backwards = false) const {return new FHashtableIterator<KeyType,ValueType>(*this, backwards ? _iterTail : _iterHead, backwards);}

   /** Get an iterator for use with this table, starting at the given entry.
     * @param startAt The key in this table to start the iteration at.
     * @param backwards Set this to true if you want to iterate through the item list backwards.  Defaults to false.
     * @return an iterator object that can be used to examine the items in the hash table, starting at
     *         the specified key.  If the specified key is not in this table, an empty iterator will be returned.
     */
   FHashtableIterator<KeyType,ValueType> GetIteratorAt(const KeyType & startAt, bool backwards = false) const {return FHashtableIterator<KeyType,ValueType>(*this, GetEntry(startAt, NULL), backwards);}

   /** Places the given (key, value) mapping into the table.  Any previous entry with
    *   a key of (key) will be replaced.  
    *  @param key The key that the new value is to be associated with.
    *  @param value The value to associate with the new key.
    *  @param setPreviousValue If there was a previously existing value associated with
    *                          (key), it will be copied into this object.
    *  @param optSetReplaced If set non-NULL, this boolean will be set to true if (setPreviousValue)
    *                        was written into, false otherwise.
    *  @return True If the operation succeeded, false if it failed (out of memory?)
    *               NOTE:  THIS BEHAVIOUR HAS CHANGED FROM THAT OF v1.64 AND BELOW!!!
    */
   bool Put(const KeyType& key, const ValueType& value, ValueType & setPreviousValue, bool * optSetReplaced = NULL);

   /** Places the given (key, value) mapping into the table.  Any previous entry with
    *  a key of (key) will be replaced.
    *  @param key The key that the new value is to be associated with.
    *  @param value The value to associate with the new key.
    *  @return True If the operation succeeded, false if it failed (out of memory?)
    *               NOTE:  THIS BEHAVIOUR HAS CHANGED FROM THAT OF v1.64 AND BELOW!!!
    */
   virtual bool Put(const KeyType& key, const ValueType& value);

   /** Removes a mapping from the table.
    *  @param key The key of the key-value mapping to remove.
    *  @return True if a key was found and the mapping removed, or false if the key wasn't found.
    */
   bool Remove(const KeyType& key);

   /** Removes the mapping with the given (key) and places its value into (setRemovedValue).
    *  @param key The key of the key-value mapping to remove.
    *  @param setRemovedValue On success, the removed value is copied into this object.
    *  @return True if a key was found and the mapping removed, or false if the key wasn't found.
    */
   bool Remove(const KeyType& key, ValueType & setRemovedValue);

   /** Removes all mappings from the hash table. */
   void Clear();

   /** You can call this to change the hash function that is used to generate integer
    *  hashing values from the key items.
    *  @param func The new hashing function to use for this table. 
    */
   void SetHashFunction(KeyHashFunc key) {_userHashFunc = func;}

   /** You can call this to set a function to use to compare keys.  Uses the same return
    *  semantics as strcmp().  By default, the Key class's '==' operator is used to compare keys,
    *  and no sorting is done.  But for some key types (e.g. const char *'s) '==' is not useful, 
    *  so you can supply your own here.  This method can also be used to activate auto-sorting
    *  of any new keys that are placed into the table.  Note that auto-sorting assumes that the
    *  table is already sorted at the time it is activated.
    *  @param func The new comparison function to use for this table.
    *  @param cookie This value will be passed on to the compare function, verbatim.
    *  @param keepSorted If true, we will use this function to keep the table's traversal 
    *                    list sorted as new entries are added.
    */
   void SetCompareFunction(KeyCompareFunc func, void * cookie, bool keepSorted) {_userCompareFunc = func; _compareCookie = cookie; _keepSorted = keepSorted;}

   /** This method does an efficient swapping of contents with (swapMe).  That is to say,
    *  when this method returns, (swapMe) will be identical to the old state of this 
    *  FHashtable, and this FHashtable will be identical to the old state of
    *  (swapMe).  Any active iterators on either table will swap also, becoming associated
    *  with the other table.
    *  @param swapMe The table whose contents and iterators are to be swapped with this table's.
    */
   void SwapContents(FHashtable<KeyType,ValueType> & swapMe);

   /** Moves the given entry to the head of the FHashtableIterator traversal sequence.
     * Note that calling this method is generally a bad idea of the table is in auto-sort mode,
     * as it is likely to unsort the traversal ordering and thus break auto-sorting.  However,
     * calling Sort() will restore the sort-order and make auto-sorting work again)
     * @param moveMe Key of the item to be moved to the front of the sequence.
     * @return true on success, or false if (moveMe) was not found in the table.
     */
   bool MoveToFront(const KeyType & moveMe);

   /** Moves the given entry to the tail of the FHashtableIterator traversal sequence.
     * Note that calling this method is generally a bad idea of the table is in auto-sort mode,
     * as it is likely to unsort the traversal ordering and thus break auto-sorting.  However,
     * calling Sort() will restore the sort-order and make auto-sorting work again)
     * @param moveMe Key of the item to be moved to the end of the sequence.
     * @return true on success, or false if (moveMe) was not found in the table.
     */
   bool MoveToBack(const KeyType & moveMe);

   /** Moves the given entry to the spot just in front of the other specified entry in the 
     * FHashtableIterator traversal sequence.
     * Note that calling this method is generally a bad idea of the table is in auto-sort mode,
     * as it is likely to unsort the traversal ordering and thus break auto-sorting.  However,
     * calling Sort() will restore the sort-order and make auto-sorting work again)
     * @param moveMe Key of the item to be moved.
     * @param toBeforeMe Key of the item that (moveMe) should be placed in front of.
     * @return true on success, or false if (moveMe) was not found in the table, 
     *         or was the same item as (toBeforeMe).
     */
   bool MoveToBefore(const KeyType & moveMe, const KeyType & toBeforeMe);

   /** Moves the given entry to the spot just behind the other specified entry in the 
     * FHashtableIterator traversal sequence.
     * Note that calling this method is generally a bad idea of the table is in auto-sort mode,
     * as it is likely to unsort the traversal ordering and thus break auto-sorting.  However,
     * calling Sort() will restore the sort-order and make auto-sorting work again)
     * @param moveMe Key of the item to be moved.
     * @param toBehindMe Key of the item that (moveMe) should be placed behind.
     * @return true on success, or false if (moveMe) was not found in the table, 
     *         or was the same item as (toBehindMe).
     */
   bool MoveToBehind(const KeyType & moveMe, const KeyType & toBehindMe);

   /** Forcefully sorts the iteration traversal list of this table using the comparison function 
     * previously specified for this hashtable via SetCompareFunction().
     * Note that this sort algorithm is O(n^2), so it's not efficient for very large tables.
     * @return True on success, or false if no comparison function was previously specified.
     */
   bool Sort();

   /** Forcefully sorts the iteration traversal list of this table using the given comparison function.
     * Note that this sort algorithm is O(n^2), so it's not efficient for very large tables.
     * @param func The object-comparison function to use.
     * @param optCompareCookie Optional miscellaneous value to pass on through when calling func().
     */
   void Sort(KeyCompareFunc func, void * optCompareCookie = NULL);

private:
   friend class FHashtableIterator<KeyType, ValueType>;

   class FHashtableEntry
   {
   public:
      FHashtableEntry() : _next(NULL), _iterPrev(NULL), _iterNext(NULL), _valid(false)
      {
         // empty
      }
 
      FHashtableEntry(const FHashtableEntry & rhs) : _iterPrev(NULL), _iterNext(NULL)
      {
         *this = rhs;
      }

      ~FHashtableEntry()
      {
         // empty
      }

      FHashtableEntry & operator=(const FHashtableEntry & rhs)
      {
         _hash  = rhs._hash;
         _key   = rhs._key;
         _value = rhs._value;
         _next  = rhs._next;
         _valid = rhs._valid;
         // DO NOT copy _iterPrev and _iterNext from rhs!  They must remain the same as before.
         return * this;
      }
 
      // Returns the entry to its just-created state
      void Invalidate(const KeyType & defaultKey, const ValueType & defaultValue)
      {
         // Gotta reset, so that something like a RefCount object
         // as a KeyType or ValueType, doesn't sit around without
         // releasing its held object/memory for a loooonnng time...
         _key   = defaultKey;
         _value = defaultValue;
         _valid = false;
         _next = _iterPrev = _iterNext = NULL;
         // hash is undefined, so no point in resetting it here
      }

      uint32 _hash;                   // precalculated for efficiency
      KeyType _key;                   // used for '==' checking
      ValueType _value;               // payload
      FHashtableEntry* _next;  // for making linked lists in our bucket
      FHashtableEntry* _iterPrev;  // for table iteration
      FHashtableEntry* _iterNext;  // for table iteration
      bool _valid;                    // only used for entries in our _table array, (which are by value and so can't be NULL)
   };

   // Auxilliary methods
   FHashtableEntry * PutAux(const KeyType& key, const ValueType& value, ValueType * optSetPreviousValue, bool * optReplacedFlag);
   bool RemoveAux(const KeyType& key, ValueType * setRemovedValue);

   // Linked list maintainence
   void InsertSortedIterationEntry(FHashtableEntry * e, KeyCompareFunc optFunc, void * cookie);
   void InsertIterationEntry(FHashtableEntry * e, FHashtableEntry * optBehindThisOne);
   void RemoveIterationEntry(FHashtableEntry * e);
   FHashtableEntry * GetEntry(const KeyType& key, FHashtableEntry ** optRetPrev) const;

   // FHashtableIterator's private API
   void RegisterIterator(FHashtableIterator<KeyType,ValueType> * iter) const
   {
      // add him to the head of our linked list of iterators
      iter->_prevIter = NULL;
      iter->_nextIter = _iterList;  
      if (_iterList) _iterList->_prevIter = iter;
      _iterList = iter;
   }

   void UnregisterIterator(FHashtableIterator<KeyType,ValueType> * iter) const
   {
      if (iter->_prevIter) iter->_prevIter->_nextIter = iter->_nextIter;
      if (iter->_nextIter) iter->_nextIter->_prevIter = iter->_prevIter;
      if (iter == _iterList) _iterList = iter->_nextIter;
      iter->_prevIter = iter->_nextIter = NULL;
   }

   KeyType * GetKeyFromCookie(void * c) const {return c ? &(((FHashtableEntry *)c)->_key) : NULL;}
   ValueType * GetValueFromCookie(void * c) const  {return c ? &(((FHashtableEntry *)c)->_value) : NULL;}

   void IterateCookie(void ** c, bool backwards) const 
   {
      FHashtableEntry * entry = *((FHashtableEntry **)c);
      *c = entry ? (backwards ? entry->_iterPrev : entry->_iterNext) : NULL;
   }

   uint32 NextPrime(uint32 start) const;
   bool GrowTable();

   uint32 _count;       //the size of the elements in the hashtable
   uint32 _tableSize;   //the size of the table.
   uint32 _threshold;
   float _loadFactor;
   FHashtableEntry * _table;
   FHashtableEntry * _iterHead;    // start of linked list to iterate through
   FHashtableEntry * _iterTail;    // end of linked list to iterate through
   KeyType _defaultKey;      // used for invalidating FHashtableEntry's
   ValueType _defaultValue;  // used for invalidating FHashtableEntry's
   KeyHashFunc _userHashFunc;
   KeyCompareFunc _userCompareFunc;
   bool _keepSorted;
   void * _compareCookie;
   mutable FHashtableIterator<KeyType, ValueType> * _iterList;  // list of existing iterators for this table
};

//===============================================================
// Implementation of FHashtable
// Necessary location for appropriate template instantiation.
//===============================================================
template <class KeyType, class ValueType>
FHashtable<KeyType,ValueType>::FHashtable(KeyHashFunc func, uint32 initialCapacity, float loadFactor)
   : _count(0), _tableSize(initialCapacity), _threshold((uint32)(initialCapacity*loadFactor)), _loadFactor(loadFactor), _table(NULL), _iterHead(NULL), _iterTail(NULL), _userHashFunc(func), _userCompareFunc(NULL), _keepSorted(false), _compareCookie(NULL), _iterList(NULL)
{
   // empty
}

template <class KeyType, class ValueType>
FHashtable<KeyType,ValueType>::
FHashtable(const FHashtable<KeyType,ValueType> & rhs)
   : _count(0), _tableSize(rhs._tableSize), _threshold(rhs._threshold), _loadFactor(rhs._loadFactor), _table(NULL), _iterHead(NULL), _iterTail(NULL), _userHashFunc(rhs._userHashFunc), _userCompareFunc(rhs._userCompareFunc), _keepSorted(rhs._keepSorted), _compareCookie(rhs._compareCookie), _iterList(NULL)
{
   *this = rhs;
}

template <class KeyType, class ValueType>
FHashtable<KeyType, ValueType> &
FHashtable<KeyType, ValueType> ::
operator=(const FHashtable<KeyType, ValueType> & rhs)
{
   if (this != &rhs)
   {
      Clear();
      FHashtableIterator<KeyType, ValueType> iter = rhs.GetIterator();
      KeyType * nextKey;
      while((nextKey = iter.GetNextKey()) != NULL) (void) Put(*nextKey, *iter.GetNextValue());  // no good way to handle out-of-mem here?  
   }
   return *this;
}

template <class KeyType, class ValueType>
FHashtable<KeyType,ValueType>::~FHashtable()
{
   Clear();
   delete [] _table;
}

template <class KeyType, class ValueType>
bool
FHashtable<KeyType,ValueType>::ContainsValue(const ValueType& value) const
{
   FHashtableIterator<KeyType, ValueType> iter = GetIterator();
   ValueType * v;
   while((v = iter.GetNextValue()) != NULL) if (*v == value) return true;
   return false;
}

template <class KeyType, class ValueType>
bool
FHashtable<KeyType,ValueType>::ContainsKey(const KeyType& key) const
{
   return (GetEntry(key, NULL) != NULL);
}

template <class KeyType, class ValueType>
bool
FHashtable<KeyType,ValueType>::Get(const KeyType& key, ValueType & setValue) const
{
   ValueType * ptr = Get(key);
   if (ptr)
   {
      setValue = *ptr;
      return true;
   }
   else return false;
}

template <class KeyType, class ValueType>
ValueType *
FHashtable<KeyType,ValueType>::Get(const KeyType& key) const
{
   FHashtableEntry * e = GetEntry(key, NULL);
   return e ? &e->_value : NULL;
}

template <class KeyType, class ValueType>
FHashtable<KeyType,ValueType>::FHashtableEntry *
FHashtable<KeyType,ValueType>::GetEntry(const KeyType& key, FHashtableEntry ** optRetPrev) const
{
   if (_table)
   {
      uint32 hash = (*_userHashFunc)(key);
      FHashtableEntry * e = &_table[hash % _tableSize];
      if (e->_valid)
      {
         FHashtableEntry * prev = NULL;
         while(e)
         {
            if ((e->_hash == hash)&&((_userCompareFunc) ? (_userCompareFunc(e->_key, key, _compareCookie) == 0) : (e->_key == key))) 
            {
               if (optRetPrev) *optRetPrev = prev;
               return e;
            }
            prev = e;
            e = e->_next; 
         }
      }
   }
   return NULL;
}

template <class KeyType, class ValueType>
bool 
FHashtable<KeyType,ValueType>::Sort()
{
   if (_userCompareFunc)
   {
      Sort(_userCompareFunc, _compareCookie);
      return true;
   }
   return false;   
}

template <class KeyType, class ValueType>
void 
FHashtable<KeyType,ValueType>::Sort(KeyCompareFunc func, void * cookie)
{
   FHashtableIterator<KeyType,ValueType> * saveIterList = _iterList;
   _iterList = NULL;  // avoid modifying our iterators

   // First, remove all the nodes from the iteration list and put them into our private list instead.  
   FHashtableEntry * privList = NULL;
   while(_iterHead)
   {
      FHashtableEntry * temp = _iterHead; // save since RemoveIterationEntry() will change _iterHead
      RemoveIterationEntry(_iterHead);
      temp->_iterNext = privList;
      privList = temp;
   }

   // Now we go through our list and add everything back using the given function, 
   while(privList)
   {
      FHashtableEntry * next = privList->_iterNext;
      InsertSortedIterationEntry(privList, func, cookie);
      privList = next;
   } 

   _iterList = saveIterList;  // lastly restore our iterators
}

template <class KeyType, class ValueType>
bool
FHashtable<KeyType,ValueType>::GrowTable()
{
   // 1. Initialize the scratch space for our active iterators.
   {
      FHashtableIterator<KeyType,ValueType> * nextIter = _iterList;
      while(nextIter)
      {
         nextIter->_scratchSpace[0] = nextIter->_scratchSpace[1] = NULL;  // these will hold our switch-to-on-success values
         nextIter = nextIter->_nextIter;
      }
   }
    
   // 2. Create a new, bigger table, and fill it with a copy of all of our data.
   FHashtable<KeyType,ValueType> biggerTable(_userHashFunc, NextPrime(2*_tableSize), _loadFactor);
   biggerTable.SetCompareFunction(_userCompareFunc, _compareCookie, _keepSorted);
   {
      FHashtableEntry * next = _iterHead;
      while(next)
      {
         FHashtableEntry * hisClone = biggerTable.PutAux(next->_key, next->_value, NULL, NULL);
         if (hisClone)
         {
            // Mark any iterators that will need to be redirected to point to the new nodes.
            FHashtableIterator<KeyType,ValueType> * nextIter = _iterList;
            while(nextIter)
            {
               if (nextIter->_nextKeyCookie   == next) nextIter->_scratchSpace[0] = hisClone;
               if (nextIter->_nextValueCookie == next) nextIter->_scratchSpace[1] = hisClone;
               nextIter = nextIter->_nextIter;
            }
         }
         else return false;  // oops, out of mem, too bad.  

         next = next->_iterNext;
      }
   }

   // 3. Swap contents with the bigger table, but don't swap iterator lists (we want to keep ours!)
   {
      FHashtableIterator<KeyType,ValueType> * temp = _iterList;
      _iterList = NULL;
      SwapContents(biggerTable);
      _iterList = temp;
   }

   // 4. Lastly, fix up our iterators to point to their new entries.
   {
      FHashtableIterator<KeyType,ValueType> * nextIter = _iterList;
      while(nextIter)
      {
         nextIter->_nextKeyCookie   = nextIter->_scratchSpace[0];
         nextIter->_nextValueCookie = nextIter->_scratchSpace[1];
         nextIter = nextIter->_nextIter;
      }
   }

   return true;
}

template <class KeyType, class ValueType>
void
FHashtable<KeyType,ValueType>::SwapContents(FHashtable<KeyType,ValueType> & swapMe)
{
   // Save our data to a backup
   uint32 tempCount                                                      = _count;
   uint32 tempTableSize                                                  = _tableSize;
   uint32 tempThreshold                                                  = _threshold;
   float  tempLoadFactor                                                 = _loadFactor;
   FHashtableEntry * tempTable                                    = _table;
   FHashtableEntry * tempIterHead                                 = _iterHead;
   FHashtableEntry * tempIterTail                                 = _iterTail;
   KeyHashFunc tempUserHashFunc                                          = _userHashFunc;
   KeyCompareFunc tempUserCompareFunc                                    = _userCompareFunc;
   bool tempKeepSorted                                                   = _keepSorted;
   void * tempCompareCookie                                              = _compareCookie;
   FHashtableIterator<KeyType, ValueType> * tempIterList          = _iterList;

   // Copy his data over our data.
   _count           = swapMe._count;
   _tableSize       = swapMe._tableSize;
   _threshold       = swapMe._threshold;
   _loadFactor      = swapMe._loadFactor;
   _table           = swapMe._table;
   _iterHead        = swapMe._iterHead;
   _iterTail        = swapMe._iterTail;
   _userHashFunc    = swapMe._userHashFunc;
   _userCompareFunc = swapMe._userCompareFunc;
   _keepSorted      = swapMe._keepSorted;
   _compareCookie   = swapMe._compareCookie;
   _iterList        = swapMe._iterList;

   // Copy our backup over his data.
   swapMe._count           = tempCount;
   swapMe._tableSize       = tempTableSize;
   swapMe._threshold       = tempThreshold;
   swapMe._loadFactor      = tempLoadFactor;
   swapMe._table           = tempTable;
   swapMe._iterHead        = tempIterHead;
   swapMe._iterTail        = tempIterTail;
   swapMe._userHashFunc    = tempUserHashFunc;
   swapMe._userCompareFunc = tempUserCompareFunc;
   swapMe._keepSorted      = tempKeepSorted;
   swapMe._compareCookie   = tempCompareCookie;
   swapMe._iterList        = tempIterList;

   // Lastly, swap the owners of all iterators, so that they will unregister from the correct table when they die
   {
      FHashtableIterator<KeyType,ValueType> * next = _iterList;
      while(next)
      {
         next->_owner = &swapMe;
         next = next->_nextIter;
      }
   }
   {
      FHashtableIterator<KeyType,ValueType> * next = swapMe._iterList;
      while(next)
      {
         next->_owner = this;
         next = next->_nextIter;
      }
   }
}

template <class KeyType, class ValueType>
bool 
FHashtable<KeyType,ValueType>::MoveToFront(const KeyType & moveMe)
{
   FHashtableEntry * e = GetEntry(moveMe, NULL);
   if (e == NULL) return false;
   if (e->_iterPrev)
   {
      RemoveIterationEntry(e);
      InsertIterationEntry(e, NULL);
   }
   return true;
}

template <class KeyType, class ValueType>
bool 
FHashtable<KeyType,ValueType>::MoveToBack(const KeyType & moveMe)
{
   FHashtableEntry * e = GetEntry(moveMe, NULL);
   if (e == NULL) return false;
   if (e->_iterNext)
   {
      RemoveIterationEntry(e);
      InsertIterationEntry(e, _iterTail);
   }
   return true;
}

template <class KeyType, class ValueType>
bool 
FHashtable<KeyType,ValueType>::MoveToBefore(const KeyType & moveMe, const KeyType & toBeforeMe)
{
   FHashtableEntry * e = GetEntry(moveMe, NULL);
   FHashtableEntry * f = GetEntry(toBeforeMe, NULL);
   if ((e == NULL)||(f == NULL)||(e == f)) return false;
   if (e->_iterNext != f)
   {
      RemoveIterationEntry(e);
      InsertIterationEntry(e, f->_iterPrev);
   }
   return true;
}

template <class KeyType, class ValueType>
bool 
FHashtable<KeyType,ValueType>::MoveToBehind(const KeyType & moveMe, const KeyType & toBehindMe)
{
   FHashtableEntry * d = GetEntry(toBehindMe, NULL);
   FHashtableEntry * e = GetEntry(moveMe, NULL);
   if ((d == NULL)||(e == NULL)||(d == e)) return false;
   if (e->_iterPrev != d)
   {
      RemoveIterationEntry(e);
      InsertIterationEntry(e, d);
   }
   return true;
}

// Adds (e) to the end of our iteration linked list, or to an appropriate location to maintain sorting, if sorting is enabled.
template <class KeyType, class ValueType>
void
FHashtable<KeyType,ValueType>::InsertSortedIterationEntry(FHashtableEntry * e, KeyCompareFunc optFunc, void * cookie)
{
   FHashtableEntry * insertAfter = _iterTail;  // default to appending to the end of the list
   if ((_iterHead)&&(optFunc))
   {
      // We're in sorted mode, so we'll try to place this guy in the correct position.
           if (optFunc(e->_key, _iterHead->_key, cookie) < 0) insertAfter = NULL;  // easy; append to the head of the list
      else if (optFunc(e->_key, _iterTail->_key, cookie) < 0)  // see if we're after the tail;  if so, no reason to iterate through
      {
         FHashtableEntry * prev = _iterHead;
         FHashtableEntry * next = _iterHead->_iterNext;  // more difficult;  find where to insert into the middle
         while(next)
         {
            if (optFunc(e->_key, next->_key, cookie) < 0) 
            {
               insertAfter = prev;
               break;
            }
            else 
            {
               prev = next;
               next = next->_iterNext;
            }
         }   
      }
   }
   InsertIterationEntry(e, insertAfter);
}

// Adds (e) to the our iteration linked list, behind (optBehindThis), or at the head if (optBehindThis) is NULL.
template <class KeyType, class ValueType>
void
FHashtable<KeyType,ValueType>::InsertIterationEntry(FHashtableEntry * e, FHashtableEntry * optBehindThis)
{
   e->_iterPrev = optBehindThis;
   e->_iterNext = optBehindThis ? optBehindThis->_iterNext : _iterHead;
   if (e->_iterPrev) e->_iterPrev->_iterNext = e;
                else _iterHead = e;
   if (e->_iterNext) e->_iterNext->_iterPrev = e;
                else _iterTail = e;
}

// Remove (e) from our iteration linked list
template <class KeyType, class ValueType>
void 
FHashtable<KeyType,ValueType>::RemoveIterationEntry(FHashtableEntry * e)
{
   // Update any iterators that were pointing at (e), so that they now point to the entry after e.
   FHashtableIterator<KeyType, ValueType> * next = _iterList;
   while(next)
   {
      if (next->_nextKeyCookie   == e) (void) next->GetNextKey();
      if (next->_nextValueCookie == e) (void) next->GetNextValue();
      next = next->_nextIter;
   }

   if (_iterHead == e) _iterHead = e->_iterNext;
   if (_iterTail == e) _iterTail = e->_iterPrev;
   if (e->_iterPrev) e->_iterPrev->_iterNext = e->_iterNext;
   if (e->_iterNext) e->_iterNext->_iterPrev = e->_iterPrev;
   e->_iterPrev = e->_iterNext = NULL; 
}

template <class KeyType, class ValueType>
bool
FHashtable<KeyType,ValueType>::Put(const KeyType& key, const ValueType& value)
{
   return (PutAux(key, value, NULL, NULL) != NULL);
}

template <class KeyType, class ValueType>
bool
FHashtable<KeyType,ValueType>::Put(const KeyType& key, const ValueType& value, ValueType & setPreviousValue, bool * optReplacedFlag)
{
   return (PutAux(key, value, &setPreviousValue, optReplacedFlag) != NULL);
}

template <class KeyType, class ValueType>
FHashtable<KeyType,ValueType>::FHashtableEntry *
FHashtable<KeyType,ValueType>::PutAux(const KeyType& key, const ValueType& value, ValueType * optSetPreviousValue, bool * optReplacedFlag)
{
   if (optReplacedFlag) *optReplacedFlag = false;

   if (_table == NULL)  // demand-allocate this
   {
      _table = newnothrow FHashtableEntry[_tableSize];
      if (_table == NULL) {WARN_OUT_OF_MEMORY; return NULL;}
   }

   uint32 hash = (*_userHashFunc)(key);
   uint32 index = hash % _tableSize;

   // If we already have an entry for this key in the table, we can just replace its contents
   FHashtableEntry * e = GetEntry(key, NULL);
   if (e)
   {
      if (optSetPreviousValue) *optSetPreviousValue = e->_value;
      if (optReplacedFlag)     *optReplacedFlag     = true;
      e->_value = value;
      return e;
   }

   // Rehash the table if the threshold is exceeded
   if (_count >= _threshold) return (GrowTable()) ? PutAux(key, value, optSetPreviousValue, optReplacedFlag) : NULL;

   FHashtableEntry * slot = &_table[index];
   if (slot->_valid)
   {
      // Creates new entry in linked list for this slot
      FHashtableEntry * newEntry = newnothrow FHashtableEntry();
      if (newEntry == NULL) {WARN_OUT_OF_MEMORY; return NULL;}
      newEntry->_hash  = hash;
      newEntry->_key   = key;
      newEntry->_value = value;
      newEntry->_valid = true;
      newEntry->_next  = slot->_next;
      slot->_next = newEntry;
      InsertSortedIterationEntry(newEntry, _keepSorted ? _userCompareFunc : NULL, _compareCookie);
      e = newEntry;
   }
   else
   {
      // First entry in slot; just copy data over
      slot->_hash  = hash;
      slot->_key   = key;
      slot->_value = value;  
      slot->_next  = NULL;
      slot->_valid = true;
      e = slot;
      InsertSortedIterationEntry(slot, _keepSorted ? _userCompareFunc : NULL, _compareCookie);
   }
   _count++;

   return e; 
}

template <class KeyType, class ValueType>
bool
FHashtable<KeyType,ValueType>::Remove(const KeyType& key)
{
   return RemoveAux(key, NULL);
}

template <class KeyType, class ValueType>
bool
FHashtable<KeyType,ValueType>::Remove(const KeyType& key, ValueType & setValue)
{
   return RemoveAux(key, &setValue);
}


template <class KeyType, class ValueType>
bool
FHashtable<KeyType,ValueType>::RemoveAux(const KeyType& key, ValueType * optSetValue)
{
   FHashtableEntry * prev;
   FHashtableEntry * e = GetEntry(key, &prev);
   if (e)
   {
      if (optSetValue) *optSetValue = e->_value;
      FHashtableEntry * next = e->_next;
      if (prev)
      {
         prev->_next = next;
         RemoveIterationEntry(e);
         delete e;
      }
      else
      {
         if (next) 
         {
            // Remove (e) from the iteration list (it's not in the correct position; it needs to be where (next) is)
            RemoveIterationEntry(e);

            // Replace (next) with (e) in the iteration list
            {
               e->_iterPrev = next->_iterPrev;
               e->_iterNext = next->_iterNext;
               if (e->_iterPrev) e->_iterPrev->_iterNext = e;
               if (e->_iterNext) e->_iterNext->_iterPrev = e;
               if (_iterHead == next) _iterHead = e;
               if (_iterTail == next) _iterTail = e;
            }

            // e takes over next's data
            *e = *next;
            e->_valid = true;

            // Update any iterators that were pointing at (next), so that they now point to e.
            FHashtableIterator<KeyType, ValueType> * nextIter = _iterList;
            while(nextIter)
            {
               if (nextIter->_nextKeyCookie   == next) nextIter->_nextKeyCookie   = e;
               if (nextIter->_nextValueCookie == next) nextIter->_nextValueCookie = e;
               nextIter = nextIter->_nextIter;
            }

            delete next;
         }
         else 
         {
            RemoveIterationEntry(e);
            e->Invalidate(_defaultKey, _defaultValue);
         }
      }
      _count--;
      return true;
   }
   return false;
}

template <class KeyType, class ValueType>
void
FHashtable<KeyType,ValueType>::Clear()
{
   // First go through our list of active iterators, and let them all know they are now invalid
/*   while(_iterList)
   {
      FHashtableIterator<KeyType,ValueType> * next = _iterList->_nextIter;
      *_iterList = FHashtableIterator<KeyType,ValueType>();
      _iterList = next;
   }
*/
   // Then go through our list of data and remove/reset it all
   while(_iterHead)
   {
      FHashtableEntry * next = _iterHead->_iterNext;  // save for later
      if ((_iterHead >= _table)&&(_iterHead < _table+_tableSize)) _iterHead->Invalidate(_defaultKey, _defaultValue);
                                                             else delete _iterHead;
      _iterHead = next;
   }
   _iterTail = NULL;

   _count = 0;
}

template <class KeyType, class ValueType>
uint32
FHashtable<KeyType,ValueType>::NextPrime(uint32 start) const
{
   if (start % 2 == 0) start++;
   uint32 i;
   for(; ; start += 2)
   {
      for(i = 3; i * i <= start; i += 2) if (start % i == 0) break;
      if (i * i > start) return start;
   }
}

//===============================================================
// Implementation of FHashtableIterator
//===============================================================

template <class KeyType, class ValueType>
FHashtableIterator<KeyType, ValueType>::FHashtableIterator() : _nextKeyCookie(NULL), _nextValueCookie(NULL), _owner(NULL)
{
   // empty
}

template <class KeyType, class ValueType>
FHashtableIterator<KeyType, ValueType>::FHashtableIterator(const FHashtableIterator<KeyType, ValueType> & rhs) : _owner(NULL)
{
   *this = rhs;
}

template <class KeyType, class ValueType>
FHashtableIterator<KeyType, ValueType>::FHashtableIterator(const FHashtable<KeyType, ValueType> & owner, void * startCookie, bool backwards) : _nextKeyCookie(startCookie), _nextValueCookie(startCookie), _backwards(backwards), _owner(&owner)
{
   _owner->RegisterIterator(this);
}

template <class KeyType, class ValueType>
FHashtableIterator<KeyType, ValueType>::~FHashtableIterator()
{
   if (_owner) _owner->UnregisterIterator(this);
}

template <class KeyType, class ValueType> 
FHashtableIterator<KeyType,ValueType> &
FHashtableIterator<KeyType,ValueType>:: operator=(const FHashtableIterator<KeyType,ValueType> & rhs)
{
   if (_owner != rhs._owner)
   {
      if (_owner) _owner->UnregisterIterator(this);
      _owner = rhs._owner;
      if (_owner) _owner->RegisterIterator(this);
   }
   _backwards       = rhs._backwards;
   _nextKeyCookie   = rhs._nextKeyCookie;
   _nextValueCookie = rhs._nextValueCookie;
   return *this;
}


template <class KeyType, class ValueType>
bool
FHashtableIterator<KeyType,ValueType>::HasMoreKeys() const
{
   return (_nextKeyCookie != NULL);
}

template <class KeyType, class ValueType>
bool
FHashtableIterator<KeyType,ValueType>::HasMoreValues() const
{
   return (_nextValueCookie != NULL);
}

template <class KeyType, class ValueType>
bool
FHashtableIterator<KeyType,ValueType>::GetNextKey(KeyType& key) 
{
   KeyType * ret = GetNextKey();
   if (ret) 
   {
      key = *ret;
      return true;
   }
   else return false;
}

template <class KeyType, class ValueType>
bool
FHashtableIterator<KeyType,ValueType>::GetNextValue(ValueType& val)
{
   ValueType * ret = GetNextValue();
   if (ret)
   {
      val = *ret;
      return true;
   } 
   else return false;
}

template <class KeyType, class ValueType>
bool
FHashtableIterator<KeyType,ValueType>::PeekNextKey(KeyType& key) const
{
   KeyType * ret = PeekNextKey();
   if (ret)
   {
      key = *ret;
      return true;
   }
   else return false;
}

template <class KeyType, class ValueType>
bool
FHashtableIterator<KeyType,ValueType>::PeekNextValue(ValueType& val) const
{
   ValueType * ret = PeekNextValue();
   if (ret)
   {
      val = *ret;
      return true;
   }
   else return false;
}

template <class KeyType, class ValueType>
ValueType *
FHashtableIterator<KeyType,ValueType>::GetNextValue()
{
   ValueType * val = PeekNextValue();
   if (val)
   {
      _owner->IterateCookie(&_nextValueCookie, _backwards);
      return val;
   }
   return NULL;
}

template <class KeyType, class ValueType>
KeyType *
FHashtableIterator<KeyType,ValueType>::GetNextKey() 
{
   KeyType * ret = PeekNextKey();
   if (ret)
   {
      _owner->IterateCookie(&_nextKeyCookie, _backwards);
      return ret;
   }
   return NULL;
}

template <class KeyType, class ValueType>
ValueType *
FHashtableIterator<KeyType,ValueType>::PeekNextValue() const
{
   return (_owner) ? _owner->GetValueFromCookie(_nextValueCookie) : NULL;
}

template <class KeyType, class ValueType>
KeyType *
FHashtableIterator<KeyType,ValueType>::PeekNextKey() const
{
   return (_owner) ? _owner->GetKeyFromCookie(_nextKeyCookie) : NULL;
}

#endif
