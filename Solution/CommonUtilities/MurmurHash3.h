//-----------------------------------------------------------------------------
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

#ifndef _MURMURHASH3_H_
#define _MURMURHASH3_H_

//-----------------------------------------------------------------------------
// Platform-specific functions and macros

// Microsoft Visual Studio

#if defined(_MSC_VER) && (_MSC_VER < 1600)

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef unsigned __int64 uint64_t;

// Other compilers

#else   // defined(_MSC_VER)

#include <stdint.h>

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------

void MurmurHash3_x86_32(const void* aKey, int aLength, uint32_t aSeed, void* out);

void MurmurHash3_x86_128(const void* aKey, int aLength, uint32_t aSeed, void* out);

void MurmurHash3_x64_128(const void* aKey, int aLength, uint32_t aSeed, void* out);

const unsigned int Hash(const char* aKey);
const unsigned int Hash(const std::string& aKey);
//-----------------------------------------------------------------------------

#endif  _MURMURHASH3_H_