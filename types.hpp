#pragma once
//---------------------------------------------------------------------------
#include <cstdint>
#include <cstring>
#include <cassert>
#include <ctime>
#include <tuple>
#include <atomic>
#include <ostream>
#include <algorithm>
#include <limits>
#ifdef __SSE4_2__
#include <nmmintrin.h>
#endif
//---------------------------------------------------------------------------
// HyPer
// (c) Thomas Neumann 2010
// modified 2021 Maximilian Kuschewski
//---------------------------------------------------------------------------
namespace types {
//---------------------------------------------------------------------------
namespace {
    [[maybe_unused]] const char* memmemSSE(const char* haystack, size_t len, const char* needleStr,
                          size_t needleLength) {
#ifdef __SSE4_2__
        if (len < 16 || needleLength > 16)
            return (const char*)memmem(haystack, len, needleStr, needleLength);

        union {
            __m128i v;
            char c[16];
        } needle;
        memcpy(needle.c, needleStr, needleLength);

        unsigned pos = 0;
        while (len - pos >= 16) {
            __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(haystack + pos));
            unsigned result =
                _mm_cmpestri(needle.v, needleLength, chunk, 16, _SIDD_CMP_EQUAL_ORDERED);
            if (16 - result >= needleLength)
                return haystack + pos + result;
            else
                pos += result;
        }

        // check rest
        pos = len - 16;
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(haystack + pos));
        unsigned result = _mm_cmpestri(needle.v, needleLength, chunk, 16, _SIDD_CMP_EQUAL_ORDERED);
        if (16 - result >= needleLength)
            return haystack + pos + result;
        else
            return NULL;

#else
        return (const char*)memmem(haystack, len, needleStr, needleLength);
#endif
    }
}  // namespace
//---------------------------------------------------------------------------
typedef uint64_t Tid;
//---------------------------------------------------------------------------
template <unsigned size> struct LengthSwitch {};
template<> struct LengthSwitch<1> { typedef uint8_t type; };
template<> struct LengthSwitch<2> { typedef uint16_t type; };
template<> struct LengthSwitch<4> { typedef uint32_t type; };
//---------------------------------------------------------------------------
template <unsigned maxLen> struct LengthIndicator {
   typedef typename LengthSwitch<(maxLen<256)?1:(maxLen<65536)?2:4>::type type;
};
//---------------------------------------------------------------------------
template<class T> void atomicMax(T& x,const T& y) {
   while ((y.value>x.value) && !__sync_bool_compare_and_swap(&x.value,x.value,y.value));
}
//---------------------------------------------------------------------------
template<class T> void atomicMin(T& x,const T& y) {
   while ((y.value<x.value) && !__sync_bool_compare_and_swap(&x.value,x.value,y.value));
}
//---------------------------------------------------------------------------
template<class T> void atomicAdd(T& x,const T& y) {
   __sync_fetch_and_add(&x.value,y.value);
}
//---------------------------------------------------------------------------
inline constexpr uint64_t murmurHash64(uint64_t k) {
    // MurmurHash64A
    const uint64_t m = 0xc6a4a7935bd1e995;
    const int r = 47;
    uint64_t h = 0x8445d61a4e774912 ^ (8 * m);
    k *= m;
    k ^= k >> r;
    k *= m;
    h ^= k;
    h *= m;
    h ^= h >> r;
    h *= m;
    h ^= h >> r;
    return h;
}
//---------------------------------------------------------------------------
enum TypeTag : uint8_t               {   INTEGER,   NUMERIC,   CHAR,   VARCHAR,   DATE,   TIMESTAMP, BIGINT   };
constexpr char const* TYPE_NAMES[] = { "integer", "numeric", "char", "varchar", "date", "timestamp", "bigint" };
//---------------------------------------------------------------------------
/// BigInt Class
class BigInt
{
public:
   static constexpr TypeTag TAG = BIGINT;
   int64_t value;

   BigInt() {}
   BigInt(int64_t value) : value(value) {}

   /// Hash
   inline uint64_t hash() const;
   /// Comparison
   inline bool operator==(const BigInt& n) const { return value==n.value; }
   /// Comparison
   inline bool operator!=(const BigInt& n) const { return value!=n.value; }
   /// Comparison
   inline bool operator<(const BigInt& n) const { return value<n.value; }
   /// Comparison
   inline bool operator<=(const BigInt& n) const { return value<=n.value; }
   /// Comparison
   inline bool operator>(const BigInt& n) const { return value>n.value; }
   /// Comparison
   inline bool operator>=(const BigInt& n) const { return value>=n.value; }
   /// Add
   inline BigInt operator+(const BigInt& n) const { BigInt r; r.value=value+n.value; return r; }
   /// Add
   inline BigInt& operator+=(const BigInt& n) { value+=n.value; return *this; }
   /// Sub
   inline BigInt operator-(const BigInt& n) const { BigInt r; r.value=value-n.value; return r; }
   /// Mul
   inline BigInt operator*(const BigInt& n) const { BigInt r; r.value=value*n.value; return r; }
   /// Cast
   static BigInt castString(const char* str, uint32_t strLen) {
       auto iter = str, limit = str + strLen;

       // Trim WS
       while ((iter != limit) && ((*iter) == ' ')) ++iter;
       while ((iter != limit) && ((*(limit - 1)) == ' ')) --limit;

       // Check for a sign
       bool neg = false;
       if (iter != limit) {
           if ((*iter) == '-') {
               neg = true;
               ++iter;
           } else if ((*iter) == '+') {
               ++iter;
           }
       }

       // Parse
       if (iter == limit) throw "invalid number format: found non-integer characters";

       uint64_t result = 0;
       unsigned digitsSeen = 0;
       for (; iter != limit; ++iter) {
           char c = *iter;
           if ((c >= '0') && (c <= '9')) {
               result = (result * 10) + (c - '0');
               ++digitsSeen;
           } else if (c == '.') {
               break;
           } else {
               throw "invalid number format: invalid character in integer string";
           }
       }

       if (digitsSeen > 20 || result > std::numeric_limits<int64_t>::max())
           throw "invalid number format: too many characters (64bit integers can at most consist of 20 numeric characters)";

       BigInt r;
       r.value = neg ? -result : result;
       return r;
   }
   // output
   friend std::ostream& operator<<(std::ostream& out, const BigInt& value) {
       out << value.value;
       return out;
   }
};
//---------------------------------------------------------------------------
inline BigInt modulo(BigInt x,int64_t y)
{
   return BigInt(x.value%y);
}
//---------------------------------------------------------------------------
/// Integer class
class Integer
{
public:
   static constexpr TypeTag TAG = INTEGER;
   int32_t value;

   Integer() {}
   Integer(int32_t value) : value(value) {}

   /// Hash
   inline uint64_t hash() const;
   /// Comparison
   inline bool operator==(const Integer& n) const { return value==n.value; }
   /// Comparison
   inline bool operator!=(const Integer& n) const { return value!=n.value; }
   /// Comparison
   inline bool operator<(const Integer& n) const { return value<n.value; }
   /// Comparison
   inline bool operator<=(const Integer& n) const { return value<=n.value; }
   /// Comparison
   inline bool operator>(const Integer& n) const { return value>n.value; }
   /// Comparison
   inline bool operator>=(const Integer& n) const { return value>=n.value; }
   /// Add
   inline Integer operator+(const Integer& n) const { Integer r; r.value=value+n.value; return r; }
   /// Add
   inline Integer& operator+=(const Integer& n) { value+=n.value; return *this; }
   /// Sub
   inline Integer operator-(const Integer& n) const { Integer r; r.value=value-n.value; return r; }
   /// Mul
   inline Integer operator*(const Integer& n) const { Integer r; r.value=value*n.value; return r; }
   /// Conversion to BigInt for comparisons
   inline operator BigInt() const { return BigInt(value); }
   /// Cast
   static Integer castString(const char* str, uint32_t strLen) {
       auto iter = str, limit = str + strLen;

       // Trim WS
       while ((iter != limit) && ((*iter) == ' ')) ++iter;
       while ((iter != limit) && ((*(limit - 1)) == ' ')) --limit;

       // Check for a sign
       bool neg = false;
       if (iter != limit) {
           if ((*iter) == '-') {
               neg = true;
               ++iter;
           } else if ((*iter) == '+') {
               ++iter;
           }
       }

       // Parse
       if (iter == limit) throw "invalid number format: found non-integer characters";

       int64_t result = 0;
       unsigned digitsSeen = 0;
       for (; iter != limit; ++iter) {
           char c = *iter;
           if ((c >= '0') && (c <= '9')) {
               result = (result * 10) + (c - '0');
               ++digitsSeen;
           } else if (c == '.') {
               break;
           } else {
               throw "invalid number format: invalid character in integer string";
           }
       }

       if (digitsSeen > 10 || result > std::numeric_limits<int32_t>::max())
           throw "invalid number format: too many characters (32bit integers can at most consist of 10 numeric characters)";

       Integer r;
       r.value = neg ? -result : result;
       return r;
   }
   // output
   friend std::ostream& operator<<(std::ostream& out, const Integer& value) {
       out << value.value;
       return out;
   }
};
//---------------------------------------------------------------------------
inline Integer modulo(Integer x,int32_t y)
{
   return Integer(x.value%y);
}
//---------------------------------------------------------------------------
/// A variable length string
template <unsigned maxLen> class Varchar
{

public:
   using length_t = typename LengthIndicator<maxLen>::type;
   static constexpr TypeTag TAG = VARCHAR;
   static constexpr unsigned MAX_LEN = maxLen;

   /// The length
   length_t len;
   /// The value
   char value[maxLen];

public:
   /// The length
   unsigned length() const { return len; }
   /// Hash
   inline uint64_t hash() const;
   /// The first character
   char* begin() { return value; }
   /// Behind the last character
   char* end() { return value+length(); }
   /// The first character
   const char* begin() const { return value; }
   /// Behind the last character
   const char* end() const { return value+length(); }

   /// Comparison
   bool operator==(const char* other) const { return strncmp(value,other,len)==0; }
   /// Comparison
   bool operator==(const Varchar& other) const { return (len==other.len)&&(memcmp(value,other.value,len)==0); }
   /// Comparison
   bool operator<(const Varchar& other) const;

   /// Build
   static Varchar build(const char* value) { Varchar result; strncpy(result.value,value,maxLen); result.len=strnlen(value,maxLen); return result; }
   ///Cast
   static Varchar<maxLen> castString(const char* str,uint32_t strLen) {assert(strLen<=maxLen); Varchar<maxLen> result; result.len=strLen; memcpy(result.value,str,strLen); return result;};
};
//---------------------------------------------------------------------------
template <unsigned maxLen> bool contains(const Varchar<maxLen>& str,const char* txt,unsigned len)
{
   return memmem(str.value,str.len,txt,len);
}
//---------------------------------------------------------------------------
template <unsigned maxLen> bool contains2(Varchar<maxLen>& str,const char* txt1,unsigned len1,const char* txt2,unsigned len2)
{
   auto start2=static_cast<const char*>(memmemSSE(str.value,str.len-len2,txt1,len1));
   return start2&&memmemSSE(start2+len1,str.len-(start2+len1-str.value),txt2,len2);
}
//---------------------------------------------------------------------------
template <unsigned maxLen> bool strEqual(const Varchar<maxLen>& str,const char* str2,unsigned len)
{
   return (str.len==len)&&(strncmp(str.value,str2,len)==0);
}
//---------------------------------------------------------------------------
template <unsigned maxLen> bool startsWith(const Varchar<maxLen>& str,const char* prefix,unsigned len)
{
   return (str.len>=len)&&(strncmp(str.value,prefix,len)==0);
}
//---------------------------------------------------------------------------
template <unsigned maxLen> bool endsWith(const Varchar<maxLen>& str,const char* prefix,unsigned len)
{
   return (str.len>=len)&&(strncmp(str.value+str.len-len,prefix,len)==0);
}
//---------------------------------------------------------------------------
template <unsigned maxLen> uint64_t Varchar<maxLen>::hash() const
// Hash
{
   unsigned result=0;
   for (unsigned index=0;index<len;index++)
      result=((result<<5)|(result>>27))^(static_cast<unsigned char>(value[index]));
   return result;
}
//---------------------------------------------------------------------------
template <unsigned maxLen> bool Varchar<maxLen>::operator<(const Varchar& other) const
// Comparison
{
   int c=memcmp(value,other.value,std::min(len,other.len));
   if (c<0) return true;
   if (c>0) return false;
   return len<other.len;
}
//---------------------------------------------------------------------------
template <unsigned maxLen> std::ostream& operator<<(std::ostream& out,const Varchar<maxLen>& value)
// Output
{
   for (auto iter=value.begin(),limit=value.end();iter!=limit;++iter)
      out << (*iter);
   return out;
}
//---------------------------------------------------------------------------
/// A fixed length string
template <unsigned maxLen> class Char
{
public:
   using length_t = typename LengthIndicator<maxLen>::type;
   static constexpr TypeTag TAG = CHAR;
   static constexpr unsigned MAX_LEN = maxLen;
   /// The length
   length_t len;
   /// The data
   char value[maxLen];

   /// The length
   unsigned length() const { return len; }
   /// Hash
   inline uint64_t hash() const;
   /// The first character
   char* begin() { return value; }
   /// Behind the last character
   char* end() { return value+length(); }
   /// The first character
   const char* begin() const { return value; }
   /// Behind the last character
   const char* end() const { return value+length(); }

   /// Comparison
   bool operator==(const char* other) const { return (other[0]==value[0])&&(len==strlen(other))&&(strncmp(value,other,len)==0); }
   /// Comparison
   bool operator!=(const char* other) const { return (len!=strlen(other))||(strncmp(value,other,len)!=0); }
   /// Comparison
   bool operator==(const Char& other) const { return (len==other.len)&&(memcmp(value,other.value,len)==0); }
   /// Comparison
   bool operator<(const Char& other) const;
   /// Comparison
   bool operator>(const Char& other) const;

   /// Build
   static Char build(const char* value) { Char result; memcpy(result.value,value,maxLen); result.len=strnlen(result.value,maxLen); return result; }
   /// Cast
   static Char<maxLen> castString(const char* str,uint32_t strLen) {
      while ((*str)==' ') {
         str++;
         strLen--;
      }
      assert(strLen<=maxLen); Char<maxLen> result; result.len=strLen; memcpy(result.value,str,strLen); return result;
   }
};
//---------------------------------------------------------------------------
template <unsigned maxLen> char firstChar(const Char<maxLen>& str)
{
   return str.value[0];
}
//---------------------------------------------------------------------------
template <unsigned maxLen> bool startsWith(const Char<maxLen>& str,const char* prefix,unsigned len)
{
   return (str.len>=len)&&(strncmp(str.value,prefix,len)==0);
}
//---------------------------------------------------------------------------
template <unsigned len,unsigned len2> Char<len> prefix(const Char<len2>& str)
{
   return Char<len>::build(str.value);
}
//---------------------------------------------------------------------------
template <unsigned maxLen> uint64_t Char<maxLen>::hash() const
// Hash
{
   unsigned result=0;
   for (unsigned index=0;index<len;index++)
      result=((result<<5)|(result>>27))^(static_cast<unsigned char>(value[index]));
   return result;
}
//---------------------------------------------------------------------------
template <unsigned maxLen> bool Char<maxLen>::operator<(const Char& other) const
// Comparison
{
   int c=memcmp(value,other.value,std::min(len,other.len));
   if (c<0) return true;
   if (c>0) return false;
   return len<other.len;
}
//---------------------------------------------------------------------------
template <unsigned maxLen> bool Char<maxLen>::operator>(const Char& other) const
// Comparison
{
   int c=memcmp(value,other.value,std::min(len,other.len));
   if (c<0) return false;
   if (c>0) return true;
   return len>other.len;
}
//---------------------------------------------------------------------------
/// A fixed length string
template<> class Char<1>
{
public:
   static constexpr TypeTag TAG = CHAR;
   static constexpr unsigned MAX_LEN = 1;
   /// The value
   char value;

public:
   /// Hash
   inline uint64_t hash() const;
   /// The length
   unsigned length() const { return value!=' '; }
   /// The first character
   char* begin() { return &value; }
   /// Behind the last character
   char* end() { return &value+length(); }
   /// The first character
   const char* begin() const { return &value; }
   /// Behind the last character
   const char* end() const { return &value+length(); }

   /// Comparison
   bool operator==(const char* other) const { return (value==other[0])&&(strlen(other)==1); }
   /// Comparison
   bool operator==(const Char& other) const { return value==other.value; }
   /// Comparison
   bool operator<(const Char& other) const { return value<other.value; }

   /// Build
   static Char build(const char* value) { Char result; result.value=*value; return result; }
   static Char<1> castString(const char* str,uint32_t strLen) {
      Char<1> x;
      x.value=str[0];
      return x;
   }
};
//---------------------------------------------------------------------------
template <unsigned maxLen=1> inline bool strEqual(const Char<1>& str,const char* str2,unsigned /*len*/)
{
   return str.value==str2[0];
}
//---------------------------------------------------------------------------
template <unsigned maxLen> inline bool strEqual(const Char<maxLen>& str,const char* str2,unsigned len)
{
   return (str.len==len)&&(strncmp(str.value,str2,len)==0);
}
//---------------------------------------------------------------------------
uint64_t Char<1>::hash() const
{
   uint64_t r=88172645463325252ull^value;
   r^=(r<<13);
   r^=(r>>7);
   return (r^=(r<<17));
}
//---------------------------------------------------------------------------
template <unsigned maxLen> std::ostream& operator<<(std::ostream& out,const Char<maxLen>& value)
// Output
{
   for (auto iter=value.begin(),limit=value.end();iter!=limit;++iter)
      out << (*iter);
   return out;
}
//---------------------------------------------------------------------------
static constexpr uint64_t numericShifts[19]={1ull,10ull,100ull,1000ull,10000ull,100000ull,1000000ull,10000000ull,100000000ull,1000000000ull,10000000000ull,100000000000ull,1000000000000ull,10000000000000ull,100000000000000ull,1000000000000000ull,10000000000000000ull,100000000000000000ull,1000000000000000000ull};
/// A numeric value
template <unsigned len,unsigned precision> class Numeric
{
public:
   static constexpr TypeTag TAG = NUMERIC;
   static constexpr unsigned LENGTH = len;
   static constexpr unsigned PRECISION = precision;
   /// The value
   int64_t value;

   template <unsigned l, unsigned p>
   friend class Numeric;

   Numeric() : value(0) {}
   Numeric(Integer x) : value(x.value*numericShifts[precision]) {}
   Numeric(int64_t x) : value(x) {}

   /// Assign a value
   void assignRaw(long v) { value=v; }
   /// Get the value
   long getRaw() const { return value; }

   /// Hash
   inline uint64_t hash() const;
   /// Comparison
   bool operator==(const Numeric<len,precision>& n) const { return value==n.value; }
   /// Comparison
   bool operator!=(const Numeric<len,precision>& n) const { return value!=n.value; }
   /// Comparison
   bool operator<(const Numeric<len,precision>& n) const { return value<n.value; }
   /// Comparison
   bool operator<=(const Numeric<len,precision>& n) const { return value<=n.value; }
   /// Comparison
   bool operator>(const Numeric<len,precision>& n) const { return value>n.value; }
   /// Comparison
   bool operator>=(const Numeric<len,precision>& n) const { return value>=n.value; }
   /// Add
   Numeric operator+(const Numeric<len,precision>& n) const { Numeric r; r.value=value+n.value; return r; }
   /// Add
   Numeric& operator+=(const Numeric<len,precision>& n) {
       __int128_t result = value;
       result += n.value;
       assert(result <= ((__int128_t) std::numeric_limits<int64_t>::max()));
       value = result;
       return *this;
   }
   /// Sub
   Numeric operator-(const Numeric<len,precision>& n) const { Numeric r; r.value=value-n.value; return r; }
   /// Div
   Numeric<len,precision> operator/(const Integer& n) const { Numeric r; r.value=value/n.value; return r; }
   /// Div
   template <unsigned l> Numeric<len,precision> operator/(const Numeric<l,0>& n) const { Numeric r; r.value=value/n.value; return r; }
   /// Div
   template <unsigned l> Numeric<len,precision> operator/(const Numeric<l,1>& n) const { return divSafe<l, 1, 10>(n); }
   /// Div
   template <unsigned l> Numeric<len,precision> operator/(const Numeric<l,2>& n) const { return divSafe<l, 2, 100>(n); }
   /// Div
   template <unsigned l> Numeric<len,precision> operator/(const Numeric<l,4>& n) const { return divSafe<l, 4, 10000>(n); }
   /// Mul
   Numeric<len,precision+precision> operator*(const Numeric<len,precision>& n) const {
       Numeric<len,precision+precision> r;
       r.value=value*n.value;
       return r;
   }
   /// Neg
   Numeric operator-() { Numeric n; n.value=-value; return n; }

   /// Cast
   template <unsigned l> Numeric<l,precision> castS() const { Numeric<l,precision> r; r.value=value; return r; }
   /// Cast
   template <unsigned l> Numeric<l,precision+1> castP1() const { Numeric<l,precision+1> r; r.value=value*10; return r; }
   /// Cast
   Numeric<len,precision+2> castP2() const { Numeric<len,precision+2> r; r.value=value*100; return r; }
   /// Cast
   template <unsigned l> Numeric<l,precision-1> castM1() const { Numeric<l,precision-1> r; r.value=value/10; return r; }
   /// Cast
   template <unsigned l> Numeric<l,precision-2> castM2() const { Numeric<l,precision-2> r; r.value=value/100; return r; }
   /// Build a number
   static Numeric<len,precision> buildRaw(long v) { Numeric r; r.value=v; return r; }
   /// division helper with overflow check for intermediary result
   template<unsigned l, unsigned p, unsigned mul>
   inline Numeric<l,p> divSafe(const Numeric<l, p>& n) const {
       Numeric<l,p> r;
       __int128_t interm = value;
       interm *= mul; interm /= n.value;
       assert(interm <= ((__int128_t) std::numeric_limits<int64_t>::max()));
       r.value = interm;
       return r;
   }
   /// Cast
   static Numeric<len,precision> castString(const char* str,uint32_t strLen) {
      auto iter=str,limit=str+strLen;

      // Trim WS
      while ((iter!=limit)&&((*iter)==' ')) ++iter;
      while ((iter!=limit)&&((*(limit-1))==' ')) --limit;

      // Check for a sign
      bool neg=false;
      if (iter!=limit) {
         if ((*iter)=='-') {
            neg=true;
            ++iter;
         } else if ((*iter)=='+') {
            ++iter;
         }
      }

      // Parse
      if (iter==limit)
         throw "invalid number format: found non-numeric characters";

      int64_t result=0;
      bool fraction=false;
      unsigned digitsSeen=0;
      unsigned digitsSeenFraction=0;
      for (;iter!=limit;++iter) {
         char c=*iter;
         if ((c>='0')&&(c<='9')) {
            if (fraction) {
               result=(result*10)+(c-'0');
               ++digitsSeenFraction;
            } else {
               ++digitsSeen;
               result=(result*10)+(c-'0');
            }
         } else if (c=='.') {
            if (fraction)
               throw "invalid number format: already in fraction";
            while ((iter!=limit)&&((*(limit-1))=='0')) --limit; // skip trailing 0s
            fraction=true;
         } else {
            throw "invalid number format: invalid character in numeric string";
         }
      }

      if ((digitsSeen>18/*(len-precision)*/)||(digitsSeenFraction>precision))
         throw "invalid number format: loosing precision";

      result*=numericShifts[precision-digitsSeenFraction];
      if (neg) {
         return buildRaw(-result);
      } else {
         return buildRaw(result);
      }
   }
};
//---------------------------------------------------------------------------
template <unsigned len,unsigned precision> uint64_t Numeric<len,precision>::hash() const
// Hash
{
   uint64_t r=88172645463325252ull^value;
   return murmurHash64(r);
   // original
   // uint64_t r=88172645463325252ull^value;
   // r^=(r<<13);
   // r^=(r>>7);
   // return (r^=(r<<17));
}
//---------------------------------------------------------------------------
template <unsigned len,unsigned precision> std::ostream& operator<<(std::ostream& out,const Numeric<len,precision>& value)
// Dump the value
{
   long v=value.getRaw();
   if (v<0) {
      out << '-';
      v=-v;
   }
   if (precision==0) {
      out << v;
   } else {
      long sep=10;
      for (unsigned index=1,limit=precision;index<limit;index++)
         sep*=10;
      out << (v/sep);
      out << '.';
      v=v%sep;
      if (!v) {
         for (unsigned index=0,limit=precision;index<limit;index++)
            out << '0';
      } else {
         while (sep>(10*v)) {
            out << '0';
            sep/=10;
         }
         out << v;
      }
   }
   return out;
}
//---------------------------------------------------------------------------
// Split ms since midnight
static void splitTime(unsigned value, unsigned& hour, unsigned& minute, unsigned& second,
                      unsigned& ms) {
    ms = value % 1000;
    value /= 1000;
    second = value % 60;
    value /= 60;
    minute = value % 60;
    value /= 60;
    hour = value % 24;
}
//---------------------------------------------------------------------------
// Merge into ms since midnight
[[maybe_unused]] 
static unsigned mergeTime(unsigned hour, unsigned minute, unsigned second, unsigned ms) {
    return ms + (1000 * second) + (60 * 1000 * minute) + (60 * 60 * 1000 * hour);
}
//---------------------------------------------------------------------------
// Algorithm from the Calendar FAQ
static unsigned mergeJulianDay(unsigned year, unsigned month, unsigned day) {
    unsigned a = (14 - month) / 12;
    unsigned y = year + 4800 - a;
    unsigned m = month + (12 * a) - 3;

    return day + ((153 * m + 2) / 5) + (365 * y) + (y / 4) - (y / 100) + (y / 400) - 32045;
}
//---------------------------------------------------------------------------
// Algorithm from the Calendar FAQ
static void splitJulianDay(unsigned jd, unsigned& year, unsigned& month, unsigned& day) {
    unsigned a = jd + 32044;
    unsigned b = (4 * a + 3) / 146097;
    unsigned c = a - ((146097 * b) / 4);
    unsigned d = (4 * c + 3) / 1461;
    unsigned e = c - ((1461 * d) / 4);
    unsigned m = (5 * e + 2) / 153;

    day = e - ((153 * m + 2) / 5) + 1;
    month = m + 3 - (12 * (m / 10));
    year = (100 * b) + d - 4800 + (m / 10);
}
//---------------------------------------------------------------------------
/// A date
class Date
{
   public:
   static constexpr TypeTag TAG = DATE;
   int32_t value;

   Date() {}
   Date(int32_t value) : value(value) {}

   /// Hash
   inline uint64_t hash() const;
   /// Comparison
   inline bool operator==(const Date& n) const { return value==n.value; }
   /// Comparison
   inline bool operator!=(const Date& n) const { return value!=n.value; }
   /// Comparison
   inline bool operator<(const Date& n) const { return value<n.value; }
   /// Comparison
   inline bool operator<=(const Date& n) const { return value<=n.value; }
   /// Comparison
   inline bool operator>(const Date& n) const { return value>n.value; }
   /// Comparison
   inline bool operator>=(const Date& n) const { return value>=n.value; }
   /// Output
   friend std::ostream& operator<<(std::ostream& out, const Date& value) {
       unsigned year, month, day;
       splitJulianDay(value.value, year, month, day);
       char buffer[30];
       snprintf(buffer, sizeof(buffer), "%04u-%02u-%02u", year, month, day);
       return out << buffer;
   }
   /// Cast
   static Date castString(const char* str, uint32_t strLen) {
       auto iter = str, limit = str + strLen;
       // Trim WS
       while ((iter != limit) && ((*iter) == ' ')) ++iter;
       while ((iter != limit) && ((*(limit - 1)) == ' ')) --limit;
       // Year
       unsigned year = 0;
       while (true) {
           if (iter == limit) throw "invalid date format";
           char c = *(iter++);
           if (c == '-') break;
           if ((c >= '0') && (c <= '9')) {
               year = 10 * year + (c - '0');
           } else
               throw "invalid date format";
       }
       // Month
       unsigned month = 0;
       while (true) {
           if (iter == limit) throw "invalid date format";
           char c = *(iter++);
           if (c == '-') break;
           if ((c >= '0') && (c <= '9')) {
               month = 10 * month + (c - '0');
           } else
               throw "invalid date format";
       }
       // Day
       unsigned day = 0;
       while (true) {
           if (iter == limit) break;
           char c = *(iter++);
           if ((c >= '0') && (c <= '9')) {
               day = 10 * day + (c - '0');
           } else
               throw "invalid date format";
       }
       // Range check
       if ((year > 9999) || (month < 1) || (month > 12) || (day < 1) || (day > 31))
           throw "invalid date format";
       Date d;
       d.value = mergeJulianDay(year, month, day);
       return d;
   }

   static Integer extractYear(const Date& d) {
       unsigned year, month, day;
       splitJulianDay(d.value, year, month, day);
       Integer r(year);
       return r;
   }
};
//---------------------------------------------------------------------------
/// A timestamp
class Timestamp
{
public:
   static constexpr TypeTag TAG = TIMESTAMP;
   /// The value
   uint64_t value;

   Timestamp() {}
   Timestamp(uint64_t value) : value(value) {}

   /// NULL
   static Timestamp null()
   {
       Timestamp result;
       result.value = 0;
       return result;
   }

   /// The value
   unsigned getRaw() const { return value; }

   /// Hash
   inline uint64_t hash() const;
   /// Comparison
   bool operator==(const Timestamp& t) const { return value==t.value; }
   /// Comparison
   bool operator!=(const Timestamp& t) const { return value!=t.value; }
   /// Comparison
   bool operator<(const Timestamp& t) const { return value<t.value; }
   /// Comparison
   bool operator>(const Timestamp& t) const { return value>t.value; }
   /// Cast
   static Timestamp castString(const char* str,uint32_t strLen);

   /// Output
   friend std::ostream& operator<<(std::ostream& out, const Timestamp& value) {

       static const uint64_t msPerDay = 24*60*60*1000;
       if (value == Timestamp::null()) out << "NULL";

       unsigned year, month, day;
       splitJulianDay(value.value / msPerDay, year, month, day);
       unsigned hour, minute, second, ms;
       splitTime(value.value % msPerDay, hour, minute, second, ms);

       char buffer[50];
       if (ms)
           snprintf(buffer, sizeof(buffer), "%04u-%02u-%02u %u:%02u:%02u.%03u", year, month, day,
                    hour, minute, second, ms);
       else
           snprintf(buffer, sizeof(buffer), "%04u-%02u-%02u %u:%02u:%02u", year, month, day, hour,
                    minute, second);
       return out << buffer;
   }
};
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------

/*
template<class T> inline uint64_t hashKey(T x) {
   uint64_t r=88172645463325252ull^x.value;
   r^=(r<<13);
   r^=(r>>7);
   return (r^=(r<<17));
}
//---------------------------------------------------------------------------
template<class T1,class T2> inline uint64_t hashKey(T1 x,T2 y) {
   uint64_t r=(88172645463325252ull^x.value)^(15240724006086445487ull^y.value);
   r^=(r<<13);
   r^=(r>>7);
   return (r^=(r<<17));
}
//---------------------------------------------------------------------------
template<class T1,class T2,class T3> inline uint64_t hashKey(T1 x,T2 y,T3 z) {
   uint64_t r=(88172645463325252ull^x.value)^(15240724006086445487ull^y.value)^(15278203968005087637ull^z.value);
   r^=(r<<13);
   r^=(r>>7);
   return (r^=(r<<17));
}
*/

//---------------------------------------------------------------------------
uint64_t Integer::hash() const
// Hash
{
   uint64_t r=88172645463325252ull^value;
   return murmurHash64(r);
   // original
   // uint64_t r=88172645463325252ull^value;
   // r^=(r<<13);
   // r^=(r>>7);
   // return (r^=(r<<17));
}
//---------------------------------------------------------------------------
uint64_t Date::hash() const
// Hash
{
   uint64_t r=88172645463325252ull^value;
   return murmurHash64(r);
   // original
   // r^=(r<<13);
   // r^=(r>>7);
   // return (r^=(r<<17));
}
//---------------------------------------------------------------------------
uint64_t Timestamp::hash() const
// Hash
{
   uint64_t r=88172645463325252ull^value;
   return murmurHash64(r);
   // original
   // r^=(r<<13);
   // r^=(r>>7);
   // return (r^=(r<<17));
}
//---------------------------------------------------------------------------
template<class T> inline uint64_t hashKey(T x) {
   return x.hash();
}
//---------------------------------------------------------------------------
template<typename T, typename... Args> inline uint64_t hashKey(T first,Args... args) {
   return first.hash() ^ hashKey(args...);
}
//---------------------------------------------------------------------------
inline uint64_t hashKey(Integer x,Integer y) {
   uint64_t k=x.value|(((uint64_t)y.value)<<32);
   return murmurHash64(k);
   // const uint64_t m = 0xc6a4a7935bd1e995;
   // const int r = 47;
   // uint64_t h = 0x8445d61a4e774912 ^ (8*m);
   // k *= m;
   // k ^= k >> r;
   // k *= m;
   // h ^= k;
   // h *= m;
   // h ^= h >> r;
   // h *= m;
   // h ^= h >> r;
   // return h|(1ull<<63);
   /*uint64_t r=(88172645463325252ull^x.value)^(15240724006086445487ull^y.value);
   r^=(r<<13);
   r^=(r>>7);
   return (r^=(r<<17));*/
}
//---------------------------------------------------------------------------
inline uint64_t hashKey(Char<1> x) {
   return x.value;
}
//---------------------------------------------------------------------------
inline uint64_t hashKey(Char<1> x,Char<1> y) {
   return (static_cast<uint32_t>(x.value)<<8)|(x.value);
}
//---------------------------------------------------------------------------
template <unsigned maxLen> inline uint64_t hashKey(Varchar<maxLen> x)
{
   unsigned result=0;
   for (unsigned index=0;index<x.len;index++)
      result=((result<<5)|(result>>27))^(static_cast<unsigned char>(x.value[index]));
   return murmurHash64(result);
   // return result;
}
//---------------------------------------------------------------------------
template <unsigned l1,unsigned l2> inline uint64_t hashKey(Char<l1> x,Char<l2> y,Integer z) {
   return x.hash()|y.hash()|hashKey(z);
}
//---------------------------------------------------------------------------
template <unsigned maxLen> inline uint64_t hashKey(Char<maxLen> x)
{
   unsigned result=0;
   for (unsigned index=0;index<x.len;index++)
      result=((result<<5)|(result>>27))^(static_cast<unsigned char>(x.value[index]));
   return murmurHash64(result);
   // return result;
}
//---------------------------------------------------------------------------
template <typename T, typename F, unsigned I = 0, typename... Args>
constexpr inline T __fold_tuple(const std::tuple<Args...>& tuple, T acc_or_init, const F& fn) {
    if constexpr (I == sizeof...(Args)) {
        return acc_or_init;
    } else {
        return __fold_tuple<T, F, I + 1, Args...>(tuple, fn(acc_or_init, std::get<I>(tuple)), fn);
    }
}
//---------------------------------------------------------------------------
// extends hashKey function from Types.hpp to support tuples
template<typename... Args> inline uint64_t hashKey(std::tuple<Args...> args) {
  return __fold_tuple(args, 0ul,
    [](const uint64_t acc, const auto& val) -> uint64_t {
      return hashKey(val) ^ acc;
    });
}
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
