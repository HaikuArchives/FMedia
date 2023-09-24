/*
	class f::FString
		
*/
/*
	-----------------------------------------------------------------------
	portions Copyright 2000 Level Control Systems.
	-----------------------------------------------------------------------
	portions Copyright 1997 Michael Olivero 
	See the included licenses/Olivero.txt for details.
	-----------------------------------------------------------------------
*/ 

#include "FString.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <stdarg.h>

//============
//Constructors
//============
FString::FString(const char * str) : _buffer(NULL), _bufferLen(0), _length(0)
{
   *this = str;
}

FString::FString(const FString & str) : FFlattenable(), _buffer(NULL), _bufferLen(0), _length(0)
{
   *this = str;
}

char
FString::CharAt(uint32 loc) const
{
   return operator[](loc);
}

int
FString::CompareTo(const FString &s2) const
{
   return strcmp(Cstr(), s2.Cstr());
}

const FString &
FString::operator=(const FString & rhs)
{
   if (this != &rhs)
   {
      uint32 rhsLen = rhs.Length();
      if (rhsLen > 0)
      {
         if (EnsureBufferSize(rhsLen+1, false) == B_NO_ERROR)
         {
            strcpy(_buffer, rhs.Cstr());
            _length = rhsLen;
         }
      }
      else
      {
         if (_buffer) _buffer[0] = '\0';
         _length = 0;
      }
   }
   return *this;
}

const FString &
FString::operator=(const char * val)
{
   if ((val)&&(val[0]))
   {
      int vlen = strlen(val);
      if (EnsureBufferSize(vlen+1, false) == B_NO_ERROR)
      {
         strcpy(_buffer, val);
         _length = vlen;
      }
   }
   else
   {
      if (_buffer) _buffer[0] = '\0';
      _length = 0;
   }
   return *this;
}

const FString &
FString::operator+=(const char aChar)
{
   if (EnsureBufferSize(Length()+2, true) == B_NO_ERROR)
   {
      _buffer[_length++] = aChar;
      _buffer[_length]   = '\0';
   }
   return *this;
}

FString
FString :: Prepend(const FString & str) const
{
   return str + (*this);
}

FString 
FString :: Append(const FString & str) const
{
   return (*this) + str;
}

const FString &
FString::operator+=(const FString &other)
{
   uint32 otherLen = other.Length();
   if ((otherLen > 0)&&(EnsureBufferSize(Length() + otherLen + 1, true) == B_NO_ERROR))
   {
      strcpy(&_buffer[_length], other.Cstr());
      _length += otherLen;
   }
   return *this;
}

FString &
FString::operator<<(const FString &Rhs)
{
   return const_cast<FString&>(*this += Rhs);
}

FString &
FString::operator<<(const char *Rhs)
{
   return *this << FString(Rhs);
}

FString &
FString::operator<<(int Rhs)
{
   char buff[64];
   sprintf(buff, "%d", Rhs);
   return *this << buff;
}

FString &
FString::operator<<(float Rhs)
{
   char buff[64];
   sprintf(buff, "%.2f", Rhs);
   return *this << buff;
}

FString &
FString::operator<<(bool Rhs)
{
   const char* val = Rhs ? "true" : "false"; 
   return *this << val;
}
 

int
FString::operator==(const FString &rhs) const
{
   if (Length() != rhs.Length()) return 0;
   if (this == &rhs) return true; 
   return (strcmp(Cstr(), rhs.Cstr()) == 0);
}

int
FString::operator!=(const FString &rhs) const
{
   return !(*this == rhs);
}

int
FString::operator<(const FString &rhs) const
{
   return strcmp(Cstr(), rhs.Cstr()) < 0;
}

int
FString::operator>(const FString &rhs) const
{
   return strcmp(Cstr(), rhs.Cstr()) > 0;
}

int
FString::operator<=(const FString &rhs) const
{
   return strcmp(Cstr(), rhs.Cstr()) <= 0;
}

int
FString::operator>=(const FString & rhs) const
{
   return strcmp(Cstr(), rhs.Cstr()) >= 0;
}

char &
FString::operator[](uint32 Index)
{
   verifyIndex(Index);
   return _buffer[Index];
}

char
FString::operator[](uint32 Index) const
{
   verifyIndex(Index);
   return _buffer[Index];
}


bool
FString::EndsWith(const FString &s2) const
{ 
   return (Length() < s2.Length()) ? false : (strcmp(Cstr()+(Length()-s2.Length()), s2.Cstr()) == 0); 
}

bool
FString::Equals(const FString &s2) const
{
   return (*this == s2);
}

void
FString::Replace(char findChar, char replaceChar)
{
   if ((_buffer)&&(findChar != replaceChar))
   {
      char * c = _buffer;
      while(*c)
      {
         if (*c == findChar) *c = replaceChar;
         c++;
      }
   }
}
   
void
FString::Replace(const FString& match, const FString& replace)
{
   if (match != replace)
   {
      FString temp(*this);
      FString newFString;
 
      int loc; 
      while ((loc = temp.IndexOf(match)) != -1)
      {
         newFString += temp.Substring(0, loc);
         newFString += replace;
         temp = temp.Substring(loc + match.Length());
      } 
      newFString += temp;
      *this = newFString;
   }
}

int
FString::IndexOf(char temp) const
{ 
   return IndexOf(temp, 0);
}

int
FString::IndexOf(char ch, uint32 fromIndex) const
{
   const char * temp = (fromIndex < Length()) ? strchr(Cstr()+fromIndex, ch) : NULL; 
   return temp ? (temp - Cstr()) : -1; 
}

int
FString::IndexOf(const FString &s2) const
{
   return IndexOf(s2, 0);
}


int
FString::IndexOf(const FString &s2, uint32 fromIndex) const
{
   const char *theFind = (fromIndex < Length()) ? strstr(Cstr()+fromIndex, s2.Cstr()) : NULL;
   return theFind ? (theFind - Cstr()) : -1;
}


int
FString::LastIndexOf(char theChar) const
{
   return LastIndexOf(theChar, 0);
}

int
FString::LastIndexOf(char ch, uint32 fromIndex) const
{
   const char * lio = (fromIndex < Length()) ? strrchr(Cstr()+fromIndex, ch) : NULL;
   return lio ? (lio - Cstr()) : -1;
}

int
FString::LastIndexOf(const FString &s2) const
{
   return (s2.Length() <= Length()) ? LastIndexOf(s2, Length() - s2.Length()) : -1;
}

int
FString::LastIndexOf(const FString &s2, uint32 fromIndex) const
{
   if (s2.Length() == 0) return Length()-1;
   if (fromIndex >= Length()) return -1;
   for (int i=fromIndex; i>=0; i--) if (strncmp(Cstr()+i, s2.Cstr(), s2.Length()) == 0) return i;
   return -1;
}

bool
FString::StartsWith(const FString &s2) const
{
   return (Length() < s2.Length()) ? false : StartsWith(s2, 0); 
} 
 
bool 
FString::StartsWith(const FString &s2, uint32 offset) const 
{
   return (offset > Length() - s2.Length()) ? false : (strncmp(Cstr()+offset, s2.Cstr(), s2.Length()) == 0); 
}

FString
FString::Substring(uint32 left) const
{
   return Substring(left, Length());
}

FString
FString::Substring(uint32 left, uint32 right) const
{
   MASSERT(left <= right, "Invalid Substring range");
   MASSERT(right <= Length(), "Index out of bounds");

   FString ret;
   if (ret.EnsureBufferSize(right-left+1, true) == B_NO_ERROR)
   {
      const char * c = Cstr();
      for (uint32 i=left; i<right; i++) ret += c[i];
   }
   return ret;
}

FString
FString::ToLowerCase() const
{
   FString ret(_buffer);
   if (ret._buffer) for (uint32 i=0; i<ret.Length(); i++) ret._buffer[i] = tolower(ret._buffer[i]);
   return ret;
}

FString
FString::ToUpperCase() const
{
   FString ret(_buffer);
   if (ret._buffer) for (uint32 i=0; i<ret.Length(); i++) ret._buffer[i] = toupper(ret._buffer[i]);
   return ret;
}
 
FString 
FString::Trim() const 
{ 
   FString temp(Cstr());
   uint32 i,j; 
 
   for (i = 0; i < Length(); i++) if (!isspace(Cstr()[i])) break; 
   for (j = temp.Length() - 1; j > i; j--) if (!isspace(Cstr()[j])) break; 
   return temp.Substring(i, j + 1); 
} 

size_t FString :: FlattenedSize() const
{
   return Length()+1;   
}

void FString :: Flatten(uint8 *buffer) const
{
   strcpy((char *)buffer, Cstr());
}

status_t FString :: Unflatten(const uint8 *buf, size_t size)
{
   const char * str = (const char *) buf;
   if ((size == 0)||(str[size-1] != '\0')) return B_ERROR;

   *this = str;
   return B_NO_ERROR;  
}

uint32 FString :: HashCode() const
{
   return CFStringHashFunc(Cstr());
}

uint32 FStringHashFunc(const FString & pstr)
{ 
   return pstr.HashCode();
}


/*--- ElfHash --------------------------------------------------- 
 *  The published hash algorithm used in the UNIX ELF format 
 *  for object files. Accepts a pointer to a string to be hashed 
 *  and returns an unsigned long. 
 *  jaf:  stolen from: http://www.ddj.com/articles/1996/9604/9604b/9604b.htm?topic=algorithms
 *-------------------------------------------------------------*/ 
uint32 CFStringHashFunc(const char * const & n)  // if you can read that, you know too much C++
{
    uint32 h = 0, g; 
    const unsigned char * name = (const unsigned char *) n;
    while (*name) 
    { 
        h = (h << 4) + *name++; 
        if ((g = h & 0xF0000000) != 0) h ^= g >> 24; 
        h &= ~g; 
    } 
    return h; 
}

int CFStringCompareFunc(const char * const & s1, const char * const & s2, void *)
{
    return strcmp(s1, s2);
}

