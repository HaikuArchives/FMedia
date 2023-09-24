/*
	class FFlattenable

	BFlattenable clone
*/
/*
	-----------------------------------------------------------------------
	mainly Copyright 2000 Level Control Systems.
	-----------------------------------------------------------------------
*/ 

#ifndef	F_FFlattenable_H
#define	F_FFlattenable_H

#include <string.h>
#include <SupportDefs.h>

/** This class is an interface representing an object that knows how
 *  to save itself into an array of bytes, and recover its state from
 *  an array of bytes.
 */
class FFlattenable 
{
public:
   /** Should return true iff every object of this type has a size that is known at compile time. */
   virtual	bool		IsFixedSize() const = 0;

   /** Should return the type code identifying this type of object.  */
   virtual	type_code	TypeCode() const = 0;

   /** Should return the number of bytes needed to store this object in its current state.  */
   virtual	size_t		FlattenedSize() const = 0;

   /** 
    *  Should store this object's state into (buffer). 
    *  @param buffer The bytes to write this object's stat into.  Must be at lease FlattenedSize() bytes long.
    */
   virtual	void		Flatten(uint8 *buffer) const = 0;

   /** 
    *  Should return true iff a buffer with type_code (code) can be used to reconstruct
    *  this object's state.  Defaults to returning true iff (code) equals TypeCode().
    *  @param code A type code constant, e.g. B_RAW_TYPE or B_STRING_TYPE, or something custom.
    *  @return True iff this object can Unflatten from a buffer of the given type, false otherwise.
    */
   virtual	bool		AllowsTypeCode(type_code code) const {return (code == TypeCode());}

   /** 
    *  Should attempt to restore this object's state from the given buffer.
    *  @param buf The buffer of bytes to unflatten from.
    *  @param size Number of bytes in the buffer.
    *  @return B_NO_ERROR if the Unflattening was successful, else B_ERROR.
    */
   virtual	status_t	Unflatten(const uint8 *buf, size_t size) = 0;

   /** Destructor */
   virtual			~FFlattenable() {/* empty */}

   /** 
    * Convenience method for writing data into a byte buffer.
    * Writes data consecutively into a byte buffer.  The output buffer is
    * assumed to be large enough to hold the data.
    * @param outBuf Flat buffer to write to
    * @param writeOffset Offset into buffer to read from.  Incremented by (blockSize) on success.
    * @param copyFrom memory location to copy bytes from
    * @param blockSize number of bytes to copy
    */
   static void WriteData(uint8 * outBuf, size_t * writeOffset, const void * copyFrom, size_t blockSize)
   {
      memcpy(&outBuf[*writeOffset], copyFrom, blockSize);
      *writeOffset += blockSize;
   };
    
   /** 
    * Convenience method for safely reading bytes from a byte buffer.  (Checks to avoid buffer overrun problems)
    * @param inBuf Flat buffer to read bytes from
    * @param outputBufferBytes total size of the input buffer
    * @param readOffset Offset into buffer to read from.  Incremented by (blockSize) on success.
    * @param copyTo memory location to copy bytes to
    * @param blockSize number of bytes to copy
    * @return True if the data was successfully read, false if the data couldn't be read (because the buffer wasn't large enough)
    */
   static bool ReadData(const uint8 * inBuf, size_t inputBufferBytes, size_t * readOffset, void * copyTo, size_t blockSize)
   {
      if ((*readOffset + blockSize) > inputBufferBytes) return false;
      memcpy(copyTo, &inBuf[*readOffset], blockSize);
      *readOffset += blockSize;
      return true;
   };
};

#endif

