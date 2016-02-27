/*
   BLAKE2 reference source code package - reference C implementations

   Written in 2012 by Samuel Neves <sneves@dei.uc.pt>

   To the extent possible under law, the author(s) have dedicated all copyright
   and related and neighboring rights to this software to the public domain
   worldwide. This software is distributed without any warranty.

   You should have received a copy of the CC0 Public Domain Dedication along with
   this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/

#ifndef blake2_impl_H
#define blake2_impl_H

#include <stdint.h>
#include <string.h>

#include "utils.h"

static inline uint32_t rotl32( const uint32_t w, const unsigned c )
{
  return ( w << c ) | ( w >> ( 32 - c ) );
}

static inline uint64_t rotl64( const uint64_t w, const unsigned c )
{
  return ( w << c ) | ( w >> ( 64 - c ) );
}

static inline uint32_t rotr32( const uint32_t w, const unsigned c )
{
  return ( w >> c ) | ( w << ( 32 - c ) );
}

static inline uint64_t rotr64( const uint64_t w, const unsigned c )
{
  return ( w >> c ) | ( w << ( 64 - c ) );
}

/* prevents compiler optimizing out memset() */
static inline void secure_zero_memory( void *v, size_t n )
{
  sodium_memzero(v, n);
}

#endif
