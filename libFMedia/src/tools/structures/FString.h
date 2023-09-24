/*
	class FString
		
*/
/*
	-----------------------------------------------------------------------
	portions Copyright 2000 Level Control Systems.
	-----------------------------------------------------------------------
	portions Copyright 1997 Michael Olivero 
	See the included licenses/Olivero.txt for details.
	-----------------------------------------------------------------------
*/ 

#ifndef F_FString_H
#define F_FString_H

#include <string.h> 
#include <stdio.h> 
#include <ctype.h> 

#include "FFlattenable.h"

#include <TypeConstants.h>
#define F_STRING_TYPE 'FStr'

#define SMALL_PORTABLE_STRING_LENGTH 7  // strings shorter than this length can be stored inline, without requiring an extra new[].

#define MASSERT(x,msg) if(!(x)) {printf("ASSERTION FAILED: (%s:%i) %s\n", __FILE__,__LINE__,msg); *((uint32 *)0) = 0xdeadbeef;}
#define MCHECKPOINT printf("Reached checkpoint at %s:%i\n", __FILE__, __LINE__);

class FString;

/** A nice hashing function for use with PortableFStrings. */
uint32 FStringHashFunc(const FString &);

/** A nice hashing function for use with (const char *)'s */
uint32 CFStringHashFunc(const char * const &);

/** A function for comparing (const char *)'s -- calls strcmp() */
int CFStringCompareFunc(const char * const &, const char * const &, void *);


/** A character string class, similar to Java's java.lang.FString */
class FString : public FFlattenable {
public:
   /** Constructor.
    *  @param str If non-NULL, the initial value for this FString.
    */
   FString(const char * str = NULL);

   /** Copy Constructor. */
   FString(const FString & str);

   /** Destructor. */
   virtual ~FString() {if (_buffer != _smallBuffer) delete [] _buffer;}

   /** Assignment Operator. */
   const FString &   operator = (const char * val);

   /** Assignment Operator. */
   const FString &   operator = (const FString &Rhs);

   /** Append Operator. 
    *  @param Rhs A string to append to this string.
    */
   const FString &   operator +=(const FString &Rhs);

   /** Append Operator.
    *  @param ch A character to append to this string.
    */
   const FString &   operator +=(const char ch);
   
   /** Append 'Stream' Operator.
    *  @param Rhs A FString to append to this string.
    *  @return a non const FString refrence to 'this' so you can chain appends.
    */
   FString &   operator <<(const FString& Rhs);   

   /** Append 'Stream' Operator.
    *  @param Rhs A const char* to append to this string.
    *  @return a non const FString refrence to 'this' so you can chain appends.
    */
   FString &   operator <<(const char* Rhs);   
   
   /** Append 'Stream' Operator.
    *  @param Rhs An int to append to this string.
    *  @return a non const FString refrence to 'this' so you can chain appends.
    */
   FString &   operator <<(int Rhs);   

   /** Append 'Stream' Operator.
    *  @param Rhs A float to append to this string. Formatting is set at 2 decimals of precision.
    *  @return a non const FString refrence to 'this' so you can chain appends.
    */
   FString &   operator <<(float Rhs);   

   /** Append 'Stream' Operator.
    *  @param Rhs A bool to append to this string. Converts to 'true' and 'false' strings appropriately.
    *  @return a non const FString refrence to 'this' so you can chain appends.
    */
   FString &   operator <<(bool Rhs);

   /** Comparison Operator.  Returns true if the two strings are equal (as determined by strcmp()) */
   int            operator ==(const FString &Rhs) const;

   /** Comparison Operator.  Returns true if the two strings are not equal (as determined by strcmp()) */
   int            operator !=(const FString &Rhs) const;

   /** Comparison Operator.  Returns true if this string comes before (Rhs) lexically. */
   int            operator < (const FString &Rhs) const;

   /** Comparison Operator.  Returns true if this string comes after (Rhs) lexically. */
   int            operator > (const FString &Rhs) const;

   /** Comparison Operator.  Returns true if the two strings are equal, or this string comes before (Rhs) lexically. */
   int            operator <=(const FString &Rhs) const;

   /** Comparison Operator.  Returns true if the two strings are equal, or this string comes after (Rhs) lexically. */
   int            operator >=(const FString &Rhs) const;

   /** Array Operator.  Used to get easy access to the characters that make up this string.
    *  @param Index Index of the character to return.  Be sure to only use valid indices!
    */
   char           operator [](uint32 Index) const;

   /** Array Operator.  Used to get easy access to the characters that make up this string.
    *  @param Index Index of the character to set.  Be sure to only use valid indices!
    */
   char&          operator [](uint32 Index);

   /** Returns the character at the (index)'th position in the string.
    *  @param index A value between 0 and (Length()-1), inclusive.
    *  @return A character value.
    */
   char           CharAt(uint32 index) const;
 
   /** Compares this string to another string using strcmp() */
   int            CompareTo(const FString &anotherFString) const;

   /** Returns a C-style pointer to our held character string. */
   const char*    Cstr() const {return _buffer ? _buffer : "";}

   /** Convenience synonym for Cstr(). */
   const char * operator()() const {return Cstr();}  

   /** Returns true iff this string ends with (suffix) */
   bool           EndsWith(const FString &suffix) const;

   /** Returns true iff this string is equal to (string), as determined by strcmp(). */
   bool           Equals(const FString &string) const;

   /** Returns the first index of (ch) in this string, or -1 if not found. */
   int            IndexOf(char ch) const;

   /** Returns the first index of (ch) in this string starting at or after (fromIndex), or -1 if not found. */
   int            IndexOf(char ch, uint32 fromIndex) const;

   /** Returns the first index of substring (str) in this string, or -1 if not found. */
   int            IndexOf(const FString &str) const;

   /** Returns the first index of substring (str) in this string starting at or after (fromIndex), or -1 if not found. */
   int            IndexOf(const FString &str, uint32 fromIndex) const;

   /** Returns the last index of (ch) in this string. */
   int            LastIndexOf(char ch) const;

   /** Returns the last index of (ch) in this string starting at or after (fromIndex), or -1 if not found. */
   int            LastIndexOf(char ch, uint32 fromIndex) const;

   /** Returns the last index of substring (str) in this string, or -1 if not found. */
   int            LastIndexOf(const FString &str) const;

   /** Returns the last index of substring (str) in this string starting at or after (fromIndex), or -1 if not found. */
   int            LastIndexOf(const FString &str, uint32 fromIndex) const;

   /** Returns the number of characters in the string (not including the terminating NUL byte) */
   const uint32   Length() const { return _length; }

   /** Returns true iff this string starts with (prefix) */
   bool           StartsWith(const FString &prefix) const;

   /** Returns true iff this string starts with the first (tooffset) characters of (prefix) */
   bool           StartsWith(const FString &prefix, uint32 toffset) const; 

   /** Returns a string that consists of (str) prepended to this string.  Does not modify the FString it is called on. */
   FString         Prepend(const FString & str) const;

   /** Returns a string that consists of (str) appended to this string.  Does not modify the FString it is called on. */
   FString         Append(const FString & str) const;

   /** Returns a string that consists of only the last part of this string, starting with index (beginIndex).  Does not modify the string it is called on*/
   FString         Substring(uint32 beginIndex) const; 

   /** Returns a string that consists of only the characters in this string from range (beginIndex) to (endIndex-1).  Does not modify the string it is called on*/
   FString         Substring(uint32 beginIndex, uint32 endIndex) const; 

   /** Returns an all lower-case version of this string.  Does not modify the string it is called on. */
   FString         ToLowerCase() const; 

   /** Returns an all upper-case version of this string.  Does not modify the string it is called on. */
   FString         ToUpperCase() const; 

   /** Returns an version of this string that has all leading and trailing whitespace removed.  Does not modify the string it is called on. */
   FString         Trim() const;  

   /** Like CompareTo(), but case insensitive. */
   int            CompareToIgnoreCase(const FString &s) const             {return ToLowerCase().CompareTo(s.ToLowerCase());}

   /** Like EndsWith(), but case insensitive. */
   bool           EndsWithIgnoreCase(const FString &s) const              {return ToLowerCase().EndsWith(s.ToLowerCase());}

   /** Like Equals(), but case insensitive. */
   bool           EqualsIgnoreCase(const FString &s) const                {return ToLowerCase().Equals(s.ToLowerCase());}

   /** Like IndexOf(), but case insensitive. */
   int            IndexOfIgnoreCase(const FString &s) const               {return ToLowerCase().IndexOf(s.ToLowerCase());}

   /** Like IndexOf(), but case insensitive. */
   int            IndexOfIgnoreCase(const FString &s, uint32 f) const     {return ToLowerCase().IndexOf(s.ToLowerCase(),f);}

   /** Like IndexOf(), but case insensitive. */
   int            IndexOfIgnoreCase(char ch) const                               {return ToLowerCase().IndexOf(tolower(ch));}

   /** Like IndexOf(), but case insensitive. */
   int            IndexOfIgnoreCase(char ch, uint32 f) const                     {return ToLowerCase().IndexOf(tolower(ch),f);}

   /** Like LastIndexOf(), but case insensitive. */
   int            LastIndexOfIgnoreCase(const FString &s) const           {return ToLowerCase().LastIndexOf(s.ToLowerCase());}

   /** Like LastIndexOf(), but case insensitive. */
   int            LastIndexOfIgnoreCase(const FString &s, uint32 f) const {return ToLowerCase().LastIndexOf(s.ToLowerCase(),f);}

   /** Like LastIndexOf(), but case insensitive. */
   int            LastIndexOfIgnoreCase(char ch) const                           {return ToLowerCase().LastIndexOf(tolower(ch));}

   /** Like LastIndexOf(), but case insensitive. */
   int            LastIndexOfIgnoreCase(char ch, uint32 f) const                 {return ToLowerCase().LastIndexOf(tolower(ch),f);}

   /** Like StartsWith(), but case insensitive. */
   bool           StartsWithIgnoreCase(const FString &s) const            {return ToLowerCase().StartsWith(s.ToLowerCase());}

   /** Like StartsWith(), but case insensitive. */
   bool           StartsWithIgnoreCase(const FString &s, uint32 o) const  {return ToLowerCase().StartsWith(s.ToLowerCase(),o);}

   /** Returns a hash code for this string */
   uint32 HashCode() const;

   /** Replaces all instances of (oldChar) in this string with (newChar) */
   void         Replace(char oldChar, char newChar); 

   /** Replaces all instances of (match) in this string with (replace) */
   void         Replace(const FString& match, const FString& replace); 
 
   /** Part of the PortableFlattenable interface.
    *  @return false
    */
   virtual bool            IsFixedSize() const {return false;}

   /** Part of the PortableFlattenable interface.
    *  @return F_STRING_TYPE
    */
   virtual type_code       TypeCode() const {return F_STRING_TYPE;}

   /** Part of the PortableFlattenable interface.
    *  @return Length()+1  (the +1 is for the terminating NUL byte)
    */
   virtual size_t          FlattenedSize() const;

   /** Part of the PortableFlattenable interface.  Flattens our string into (buffer).
    *  @param buffer A byte array to receive the flattened version of this string.
    *                There must be at least FlattenedSize() bytes available in this array.
    *  The clever secret here is that a flattened FString is just a C-style
    *  null-terminated character array, and can be used interchangably as such.
    */
   virtual void            Flatten(uint8 *buffer) const;

   /** Unflattens a FString from (buf).
    *  @param buf an array of (size) bytes.
    *  @size the number of bytes in (buf).
    *  @return B_NO_ERROR (never fails!)
    */
   virtual status_t        Unflatten(const uint8 *buf, size_t size);

private:
   char _smallBuffer[SMALL_PORTABLE_STRING_LENGTH+1];  // store very small strings in-line, to avoid dynamic allocation
   char * _buffer;            // Stores the chars.  May point at (_smallBuffer), or a dynamically allocated buffer
   uint32 _bufferLen;         // Number of bytes pointed to by (_buffer)
   uint32 _length;            // cached strlen(_buffer)

   status_t EnsureBufferSize(uint32 newBufLen, bool retainValue);
   void verifyIndex(uint32 number) const;
};

// This method tries to ensure that at least (newBufLen) chars
// are available for storing data in.  (requestedBufLen) should include
// the terminating NUL.  If (retainValue) is true, the current string value
// will be retained; otherwise it should be set right after this call
// returns...
inline status_t
FString::EnsureBufferSize(uint32 requestedBufLen, bool retainValue)
{
   if ((requestedBufLen > 0)&&((_buffer == NULL)||(requestedBufLen > _bufferLen)))
   {
      // If we're doing an initial allocation, just allocate the bytes requested.
      // If it's a re-allocation, allocate more than requested as it's more likely
      // to happen yet another time...
      uint32 newBufLen = (_buffer == NULL) ? requestedBufLen : (requestedBufLen * 2);
      char * newBuf = (requestedBufLen <= sizeof(_smallBuffer)) ? _smallBuffer : new char[newBufLen]; 
      if (newBuf == _smallBuffer) newBufLen = sizeof(_smallBuffer);
      if (newBuf)
      {
         if ((retainValue)&&(_buffer)&&(newBuf != _buffer)) strncpy(newBuf, _buffer, newBufLen);
         if (_buffer != _smallBuffer) delete [] _buffer;
         _buffer = newBuf;
         _bufferLen = newBufLen;
      }
      else 
      {
         return B_ERROR;
      }
   }
   return B_NO_ERROR;
}

inline void 
FString::verifyIndex(uint32 index) const 
{ 
   MASSERT(index < _length, "Index Out Of Bounds Exception"); 
} 

//Friend Functions
//================
inline const FString
operator+(const FString & Lhs, const FString &Rhs) 
{
   FString ret(Lhs);
   ret += Rhs;
   return ret; 
}

inline const FString
operator+(const FString & Lhs, char Rhs) 
{
   FString ret(Lhs);
   ret += Rhs;
   return ret; 
}

#endif
