#ifndef __COVER_H_
#define __COVER_H_

#include <algorithm>

namespace mtrpc {


extern const char *digits;
extern const char* zero;
extern const char* digitsHex;


// Efficient Integer to String Conversions, by Matthew Wilson.
template<typename T>
size_t convert(char buf[], T value)
{
    T i = value;
    char* p = buf;

    do
    {
        *p++ = *(zero + (i % 10));
        i /= 10;
    } while (i != 0);

    if (value < 0)
    {
        *p++ = '-';
    }
    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

template<typename T>
size_t convertHex(char buf[], T* value)
{
    unsigned long i = reinterpret_cast<unsigned long>(value);
    char* p = buf;

    do
    {
        *p++ = *(digitsHex + (i&0xF));
        i = i>>4;

    } while (i != 0);

    *p = '\0';
    std::reverse(buf, p);

    return p - buf;
}

}
#endif
