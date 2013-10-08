#ifndef PORTABLE_JANE_H
#define PORTABLE_JANE_H "+endian +uint128"
/* 0000-os-100-solaris.h */

#if defined(sun) || defined(__sun) || defined(__SVR4) || defined(__svr4__)
	#include <sys/mman.h>
	#include <sys/time.h>
	#include <fcntl.h>

	#define OS_SOLARIS
#endif

/* 0000-os-100-unix.h */

#if defined(__unix__) || defined(unix)
	#include <sys/mman.h>
	#include <sys/time.h>
	#if !defined(USG)
		#include <sys/param.h> /* need this to define BSD */
	#endif
	#include <unistd.h>
	#include <fcntl.h>

	#define OS_NIX
	#if defined(__linux__)
		#include <endian.h>
		#define OS_LINUX
	#elif defined(BSD)
		#define OS_BSD

		#if defined(MACOS_X) || (defined(__APPLE__) & defined(__MACH__))
			#define OS_OSX
		#elif defined(macintosh) || defined(Macintosh)
			#define OS_MAC
		#elif defined(__OpenBSD__)
			#define OS_OPENBSD
		#elif defined(__FreeBSD__)
			#define OS_FREEBSD
		#elif defined(__NetBSD__)
			#define OS_NETBSD
		#endif
	#endif
#endif

/* 0000-os-100-windows.h */

#if defined(_WIN32)	|| defined(_WIN64) || defined(__TOS_WIN__) || defined(__WINDOWS__)
	#include <windows.h>
	#include <wincrypt.h>
	#define OS_WINDOWS
#endif

/* 0100-compiler-000.h */

#undef NOINLINE
#undef INLINE
#undef FASTCALL
#undef CDECL
#undef STDCALL
#undef NAKED

/* 0100-compiler-100-clang.h */

#if defined(__clang__)
	#define COMPILER_CLANG ((__clang_major__ * 10000) + (__clang_minor__ * 100) + (__clang_patchlevel__))
#endif

/* 0100-compiler-100-gcc.h */

#if defined(__GNUC__)
	#if (__GNUC__ >= 3)
		#define COMPILER_GCC_PATCHLEVEL __GNUC_PATCHLEVEL__
	#else
		#define COMPILER_GCC_PATCHLEVEL 0
	#endif
	#define COMPILER_GCC ((__GNUC__ * 10000) + (__GNUC_MINOR__ * 100) + (COMPILER_GCC_PATCHLEVEL))

	#include <stdint.h>

	typedef unsigned int fpu_control_t;

	#define ROTL32(a,b) (((a) << (b)) | ((a) >> (32 - b)))
	#define ROTR32(a,b) (((a) >> (b)) | ((a) << (32 - b)))
	#define ROTL64(a,b) (((a) << (b)) | ((a) >> (64 - b)))
	#define ROTR64(a,b) (((a) >> (b)) | ((a) << (64 - b)))

	#if (COMPILER_GCC >= 30000)
		#define NOINLINE __attribute__((noinline))
	#else
		#define NOINLINE
	#endif
	#if (COMPILER_GCC >= 30000)
		#define INLINE inline __attribute__((always_inline))
	#else
		#define INLINE inline
	#endif
	#if (COMPILER_GCC >= 30400)
		#define FASTCALL __attribute__((fastcall))
	#else
		#define FASTCALL
	#endif
	#define CDECL __attribute__((cdecl))
	#define STDCALL __attribute__((stdcall))

	#define mul32x32_64(a,b) ((uint64_t)(a) * (b))
	#define mul32x32_64s(a,b) (((int64_t)(a))*(b))
#endif

/* 0100-compiler-100-icc.h */

#if defined(__ICC)
	#define COMPILER_ICC __ICC
#endif

/* 0100-compiler-100-mingw.h */

#if defined(__MINGW32__) || defined(__MINGW64__)
	#define COMPILER_MINGW
#endif

/* 0100-compiler-100-msvc.h */

#if defined(_MSC_VER)
#ifndef _CRT_SECURE_NO_WARNINGS
# define _CRT_SECURE_NO_WARNINGS
#endif

	#pragma warning(disable : 4127) /* conditional expression is constant */
	#pragma warning(disable : 4100) /* unreferenced formal parameter */

	#include <float.h>
	#include <stdlib.h> /* _rotl */
	#include <intrin.h>

	#define COMPILER_MSVC_VS6       120000000
	#define COMPILER_MSVC_VS6PP     121000000
	#define COMPILER_MSVC_VS2002    130000000
	#define COMPILER_MSVC_VS2003    131000000
	#define COMPILER_MSVC_VS2005    140050727
	#define COMPILER_MSVC_VS2008    150000000
	#define COMPILER_MSVC_VS2008SP1 150030729
	#define COMPILER_MSVC_VS2010    160000000
	#define COMPILER_MSVC_VS2010SP1 160040219
	#define COMPILER_MSVC_VS2012RC  170000000
	#define COMPILER_MSVC_VS2012    170050727

	#if _MSC_FULL_VER > 100000000
		#define COMPILER_MSVC (_MSC_FULL_VER)
	#else
		#define COMPILER_MSVC (_MSC_FULL_VER * 10)
	#endif

	#if ((_MSC_VER == 1200) && defined(_mm_free))
		#undef COMPILER_MSVC
		#define COMPILER_MSVC COMPILER_MSVC_VS6PP
	#endif

	typedef unsigned char uint8_t;
	typedef unsigned short uint16_t;
	typedef unsigned int uint32_t;
	typedef signed int int32_t;
	typedef unsigned __int64 uint64_t;
	typedef signed __int64 int64_t;

	typedef uint16_t fpu_control_t;

	#define ROTL32(a,b) _rotl(a,b)
	#define ROTR32(a,b) _rotr(a,b)
	#define ROTL64(a,b) _rotl64(a,b)
	#define ROTR64(a,b) _rotr64(a,b)

	#define NOINLINE __declspec(noinline)
	#define INLINE __forceinline
	#define FASTCALL __fastcall
	#define CDECL __cdecl
	#define STDCALL __stdcall
	#define NAKED __declspec(naked)

	#if defined(_DEBUG)
		#define mul32x32_64(a,b) (((uint64_t)(a))*(b))
		#define mul32x32_64s(a,b) (((int64_t)(a))*(b))
	#else
		#define mul32x32_64(a,b) __emulu(a,b)
		#define mul32x32_64s(a,b) __emul(a,b)
	#endif
#endif
/* 0100-compiler-999.h */

#define OPTIONAL_INLINE /* config */
#if defined(OPTIONAL_INLINE)
	#undef OPTIONAL_INLINE
	#define OPTIONAL_INLINE INLINE
#else
	#define OPTIONAL_INLINE
#endif

#define Preprocessor_ToString(s) #s
#define Stringify(s) Preprocessor_ToString(s)

#include <stdio.h>
#include <string.h>

/* 0200-cpu-100-alpha.h */

#if defined(__alpha__) || defined(__alpha) || defined(_M_ALPHA)
	#define CPU_ALPHA
#endif

/* 0200-cpu-100-hppa.h */

#if defined(__hppa__) || defined(__hppa)
	#define CPU_HPPA
#endif

/* 0200-cpu-100-intel.h */

#if defined(__amd64__) || defined(__amd64) || defined(__x86_64__ ) || defined(_M_X64)
	#define CPU_X86_64
#elif defined(__i586__) || defined(__i686__) || (defined(_M_IX86) && (_M_IX86 >= 500))
	#define CPU_X86 500
#elif defined(__i486__) || (defined(_M_IX86) && (_M_IX86 >= 400))
	#define CPU_X86 400
#elif defined(__i386__) || (defined(_M_IX86) && (_M_IX86 >= 300)) || defined(__X86__) || defined(_X86_) || defined(__I86__)
	#define CPU_X86 300
#elif defined(__ia64__) || defined(_IA64) || defined(__IA64__) || defined(_M_IA64) || defined(__ia64)
	#define CPU_IA64
#endif

/* 0200-cpu-100-ppc.h */

#if defined(powerpc) || defined(__PPC__) || defined(__ppc__) || defined(_ARCH_PPC) || defined(__powerpc__) || defined(__powerpc) || defined(POWERPC) || defined(_M_PPC)
	#define CPU_PPC
	#if defined(_ARCH_PWR7)
		#define CPU_POWER7
	#elif defined(__64BIT__)
		#define CPU_PPC64
	#else
		#define CPU_PPC32
	#endif
#endif

/* 0200-cpu-100-sparc.h */

#if defined(__sparc__) || defined(__sparc) || defined(__sparcv9)
	#define CPU_SPARC
	#if defined(__sparcv9)
		#define CPU_SPARC64
	#else
		#define CPU_SPARC32
	#endif
#endif

/* 0200-cpu-200-bits.h */

#if defined(CPU_X86_64) || defined(CPU_IA64) || defined(CPU_SPARC64) || defined(__64BIT__) || defined(__LP64__) || defined(_LP64) || (defined(_MIPS_SZLONG) && (_MIPS_SZLONG == 64))
	#define CPU_64BITS

	#undef FASTCALL
	#undef CDECL
	#undef STDCALL

	#define FASTCALL
	#define CDECL
	#define STDCALL
#endif

/* 0200-cpu-200-endian.h */

#if ((defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) && (__BYTE_ORDER == __LITTLE_ENDIAN)) || \
	 (defined(BYTE_ORDER) && defined(LITTLE_ENDIAN) && (BYTE_ORDER == LITTLE_ENDIAN)) || \
	 (defined(CPU_X86) || defined(CPU_X86_64)) || \
	 (defined(vax) || defined(MIPSEL) || defined(_MIPSEL)))
#define CPU_LE
#elif ((defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && (__BYTE_ORDER == __BIG_ENDIAN)) || \
	   (defined(BYTE_ORDER) && defined(BIG_ENDIAN) && (BYTE_ORDER == BIG_ENDIAN)) || \
	   (defined(CPU_SPARC) || defined(CPU_PPC) || defined(mc68000) || defined(sel)) || defined(_MIPSEB))
#define CPU_BE
#else
	/* unknown endian! */
#endif

#if defined(__s390__) || defined(__zarch__) || defined(__SYSC_ZARCH__)
# define CPU_Z390
#endif

/* 0400-endian-100-be.h */

#if defined(CPU_BE) && !defined(CPU_ALIGNED_ACCESS_REQUIRED)
	static INLINE uint16_t fU8TO16_BE_FAST(const uint8_t *p) { return *(const uint16_t *)p; }
	static INLINE uint32_t fU8TO32_BE_FAST(const uint8_t *p) { return *(const uint32_t *)p; }
	static INLINE uint64_t fU8TO64_BE_FAST(const uint8_t *p) {	return *(const uint64_t *)p; }
	static INLINE void fU16TO8_BE_FAST(uint8_t *p, const uint16_t v) { *(uint16_t *)p = v; }
	static INLINE void fU32TO8_BE_FAST(uint8_t *p, const uint32_t v) { *(uint32_t *)p = v; }
	static INLINE void fU64TO8_BE_FAST(uint8_t *p, const uint64_t v) { *(uint64_t *)p = v; }

	#define U8TO16_BE(p) fU8TO16_BE_FAST(p)
	#define U8TO32_BE(p) fU8TO32_BE_FAST(p)
	#define U8TO64_BE(p) fU8TO64_BE_FAST(p)
	#define U16TO8_BE(p, v) fU16TO8_BE_FAST(p, v)
	#define U32TO8_BE(p, v) fU32TO8_BE_FAST(p, v)
	#define U64TO8_BE(p, v) fU64TO8_BE_FAST(p, v)
#endif

/* 0400-endian-100-le.h */

#if defined(CPU_LE) && !defined(CPU_ALIGNED_ACCESS_REQUIRED)
	static INLINE uint16_t fU8TO16_LE_FAST(const uint8_t *p) { return *(const uint16_t *)p; }
	static INLINE uint32_t fU8TO32_LE_FAST(const uint8_t *p) { return *(const uint32_t *)p; }
	static INLINE uint64_t fU8TO64_LE_FAST(const uint8_t *p) {	return *(const uint64_t *)p; }
	static INLINE void fU16TO8_LE_FAST(uint8_t *p, const uint16_t v) { *(uint16_t *)p = v; }
	static INLINE void fU32TO8_LE_FAST(uint8_t *p, const uint32_t v) { *(uint32_t *)p = v; }
	static INLINE void fU64TO8_LE_FAST(uint8_t *p, const uint64_t v) { *(uint64_t *)p = v; }

	#define U8TO16_LE(p) fU8TO16_LE_FAST(p)
	#define U8TO32_LE(p) fU8TO32_LE_FAST(p)
	#define U8TO64_LE(p) fU8TO64_LE_FAST(p)
	#define U16TO8_LE(p, v) fU16TO8_LE_FAST(p, v)
	#define U32TO8_LE(p, v) fU32TO8_LE_FAST(p, v)
	#define U64TO8_LE(p, v) fU64TO8_LE_FAST(p, v)
#endif

/* 0400-endian-100-ppc.h */

#if defined(CPU_PPC)
	#if defined(CPU_POWER7)
		static INLINE uint64_t fU8TO64_LE_FAST(const uint8_t *p) {
			uint64_d d;
			__asm__ ("ldbrx %0,0,%1" : "=r"(d) : "r"(p))
			return d;
		}

		static INLINE void
		fU64TO8_LE_FAST(uint8_t *p, const uint64_t v) {
			__asm__ ("stdbrx %1,0,%0" : : "r"(p), "r"(v))
		}
	#elif defined(CPU_PPC64)
		static INLINE uint64_t
		fU8TO64_LE_FAST(const uint8_t *p) {
			uint64_t *s4, h, d;
			__asm__ ("addi %0,%3,4;lwbrx %1,0,%3;lwbrx %2,0,%0;rldimi %1,%2,32,0" : "+r"(s4), "=r"(d), "=r"(h) : "b"(p));
			return d;
		}

		static INLINE void
		fU64TO8_LE_FAST(uint8_t *p, const uint64_t v) {
			uint64_t *s4, h = v >> 32;
			__asm__ ("addi %0,%3,4;stwbrx %1,0,%3;stwbrx %2,0,%0" : "+r"(s4) : "r"(v), "r"(h), "b"(p));
		}
	#elif defined(CPU_PPC32)
		static INLINE uint64_t
		fU8TO64_LE_FAST(const uint8_t *p) {
			uint32_t *s4, h, l;
			__asm__ ("addi %0,%3,4;lwbrx %1,0,%3;lwbrx %2,0,%0" : "+r"(s4), "=r"(l), "=r"(h) : "b"(p));\
			return ((uint64_t)h << 32) | l;
		}

		static INLINE void
		fU64TO8_LE_FAST(uint8_t *p, const uint64_t v) {
			uint32_t *s4, h = (uint32_t)(v >> 32), l = (uint32_t)(v & (uint32_t)0xffffffff);
			__asm__ ("addi %0,%3,4;stwbrx %1,0,%3;stwbrx %2,0,%0" : "+r"(s4) : "r"(l), "r"(h), "b"(p));
		}
	#endif

	static INLINE uint32_t
	fU8TO32_LE_FAST(const uint8_t *p) {
		uint32_t d;
		__asm__ ("lwbrx %0,0,%1" : "=r"(d) : "r"(p));
		return d;
	}

	static INLINE void
	fU32TO8_LE_FAST(uint8_t *p, const uint32_t v) {
		__asm__ __volatile__("stwbrx %1,0,%0" : : "r"(p), "r"(v));
	}

	#define U8TO32_LE(p) fU8TO32_LE_FAST(p)
	#define U8TO64_LE(p) fU8TO64_LE_FAST(p)
	#define U32TO8_LE(p, v) fU32TO8_LE_FAST(p, v)
	#define U64TO8_LE(p, v) fU64TO8_LE_FAST(p, v)
#endif

/* 0400-endian-100-sparc.h */

#if defined(CPU_SPARC)
	#if defined(CPU_SPARC64)
		static INLINE uint64_t
		fU8TO64_LE_FAST(const uint8_t *p) {
			uint64_d d;
			__asm__ ("ldxa [%1]0x88,%0" : "=r"(d) : "r"(p));
			return d;
		}

		static INLINE void
		fU64TO8_LE_FAST(uint8_t *p, const uint64_t v) {
			__asm__ ("stxa %0,[%1]0x88" : : "r"(v), "r"(p));
		}
	#else
		static INLINE uint64_t
		fU8TO64_LE_FAST(const uint8_t *p) {
			uint32_t *s4, h, l;
			__asm__ ("add %3,4,%0\n\tlda [%3]0x88,%1\n\tlda [%0]0x88,%2" : "+r"(s4), "=r"(l), "=r"(h) : "r"(p));
			return ((uint64_t)h << 32) | l;
		}

		static INLINE void
		fU64TO8_LE_FAST(uint8_t *p, const uint64_t v) {
			uint32_t *s4, h = (uint32_t)(v >> 32), l = (uint32_t)(v & (uint32_t)0xffffffff);
			__asm__ ("add %3,4,%0\n\tsta %1,[%3]0x88\n\tsta %2,[%0]0x88" : "+r"(s4) : "r"(l), "r"(h), "r"(p));
		}
	#endif

	static INLINE uint32_t
	fU8TO32_LE_FAST(const uint8_t *p) {
		uint32_t d;
		__asm__ ("lda [%1]0x88,%0" : "=r"(d) : "r"(p));
		return d;
	}

	static INLINE void
	fU32TO8_LE_FAST(uint8_t *p, const uint32_t v) {
		__asm__ ("sta %0,[%1]0x88" : : "r"(p), "r"(v));
	}

	#define U8TO32_LE(p) fU8TO32_LE_FAST(p)
	#define U8TO64_LE(p) fU8TO64_LE_FAST(p)
	#define U32TO8_LE(p, v) fU32TO8_LE_FAST(p, v)
	#define U64TO8_LE(p, v) fU64TO8_LE_FAST(p, v)
#endif

/* 0400-endian-100-x86.h */

#if (((defined(CPU_X86) && (CPU_X86 >= 400)) || defined(CPU_X86_64)) && (defined(COMPILER_MSVC) || defined(COMPILER_GCC)))
	#if defined(COMPILER_MSVC)
		static INLINE uint16_t U16_SWAP_FAST(uint16_t v) { return _byteswap_ushort(v); }
		static INLINE uint32_t U32_SWAP_FAST(uint32_t v) { return _byteswap_ulong(v); }
		static INLINE uint64_t U64_SWAP_FAST(uint64_t v) { return _byteswap_uint64(v); }
	#else
		static INLINE uint16_t U16_SWAP_FAST(uint16_t v) { __asm__("rorw $8,%0" : "+r" (v)); return v; }
		static INLINE uint32_t U32_SWAP_FAST(uint32_t v) { __asm__("bswap %0" : "+r" (v)); return v; }
		#if defined(CPU_X86_64)
			static INLINE uint64_t U64_SWAP_FAST(uint64_t v) { __asm__("bswap %0" : "+r" (v)); return v; }
		#else
			static INLINE uint64_t U64_SWAP_FAST(uint64_t v) { 
				uint32_t lo = U32_SWAP_FAST((uint32_t)(v)), hi = U32_SWAP_FAST((uint32_t)(v >> 32));
				return ((uint64_t)lo << 32) | hi;
			}
		#endif
	#endif


	static INLINE uint16_t fU8TO16_BE_FAST(const uint8_t *p) { return U16_SWAP_FAST(*(const uint16_t *)p); }
	static INLINE uint32_t fU8TO32_BE_FAST(const uint8_t *p) { return U32_SWAP_FAST(*(const uint32_t *)p); }
	static INLINE uint64_t fU8TO64_BE_FAST(const uint8_t *p) { return U64_SWAP_FAST(*(const uint64_t *)p); }
	static INLINE void fU16TO8_BE_FAST(uint8_t *p, const uint16_t v) { *(uint16_t *)p = U16_SWAP_FAST(v); }
	static INLINE void fU32TO8_BE_FAST(uint8_t *p, const uint32_t v) { *(uint32_t *)p = U32_SWAP_FAST(v); }
	static INLINE void fU64TO8_BE_FAST(uint8_t *p, const uint64_t v) { *(uint64_t *)p = U64_SWAP_FAST(v); }

	#define U16_SWAP(p) U16_SWAP_FAST(p)
	#define U32_SWAP(p) U32_SWAP_FAST(p)
	#define U64_SWAP(p) U64_SWAP_FAST(p)
	#define U8TO16_BE(p) fU8TO16_BE_FAST(p)
	#define U8TO32_BE(p) fU8TO32_BE_FAST(p)
	#define U8TO64_BE(p) fU8TO64_BE_FAST(p)
	#define U16TO8_BE(p, v) fU16TO8_BE_FAST(p, v)
	#define U32TO8_BE(p, v) fU32TO8_BE_FAST(p, v)
	#define U64TO8_BE(p, v) fU64TO8_BE_FAST(p, v)
#endif

/* 0400-endian-999-generic-be.h */

#if !defined(U8TO16_BE)
	static INLINE uint16_t
	fU8TO16_BE_SLOW(const uint8_t *p) {
		return
		(((uint16_t)(p[0]) <<  8) |
		 ((uint16_t)(p[1])      ));
	}

	#define U8TO16_BE(p) fU8TO16_BE_SLOW(p)
#endif


#if !defined(U8TO32_BE)
	static INLINE uint32_t
	fU8TO32_BE_SLOW(const uint8_t *p) {
		return
		(((uint32_t)(p[0]) << 24) |
		 ((uint32_t)(p[1]) << 16) |
		 ((uint32_t)(p[2]) <<  8) |
		 ((uint32_t)(p[3])      ));
	}

	#define U8TO32_BE(p) fU8TO32_BE_SLOW(p)
#endif

#if !defined(U8TO64_BE)
	static INLINE uint64_t
	fU8TO64_BE_SLOW(const uint8_t *p) {
		return
		(((uint64_t)(p[0]) << 56) |
		 ((uint64_t)(p[1]) << 48) |
		 ((uint64_t)(p[2]) << 40) |
		 ((uint64_t)(p[3]) << 32) |
		 ((uint64_t)(p[4]) << 24) |
		 ((uint64_t)(p[5]) << 16) |
		 ((uint64_t)(p[6]) <<  8) |
		 ((uint64_t)(p[7])      ));
	}

	#define U8TO64_BE(p) fU8TO64_BE_SLOW(p)
#endif

#if !defined(U16TO8_BE)
	static INLINE void
	fU16TO8_BE_SLOW(uint8_t *p, const uint16_t v) {
		p[0] = (uint8_t)(v >>  8);
		p[1] = (uint8_t)(v      );
	}

	#define U16TO8_BE(p, v) fU16TO8_BE_SLOW(p, v)
#endif

#if !defined(U32TO8_BE)
	static INLINE void
	fU32TO8_BE_SLOW(uint8_t *p, const uint32_t v) {
		p[0] = (uint8_t)(v >> 24);
		p[1] = (uint8_t)(v >> 16);
		p[2] = (uint8_t)(v >>  8);
		p[3] = (uint8_t)(v      );
	}

	#define U32TO8_BE(p, v) fU32TO8_BE_SLOW(p, v)
#endif

#if !defined(U64TO8_BE)
	static INLINE void
	fU64TO8_BE_SLOW(uint8_t *p, const uint64_t v) {
		p[0] = (uint8_t)(v >> 56);
		p[1] = (uint8_t)(v >> 48);
		p[2] = (uint8_t)(v >> 40);
		p[3] = (uint8_t)(v >> 32);
		p[4] = (uint8_t)(v >> 24);
		p[5] = (uint8_t)(v >> 16);
		p[6] = (uint8_t)(v >>  8);
		p[7] = (uint8_t)(v      );
	}

	#define U64TO8_BE(p, v) fU64TO8_BE_SLOW(p, v)
#endif

/* 0400-endian-999-generic-le.h */

#if !defined(U8TO16_LE)
	static INLINE uint16_t
	fU8TO16_LE_SLOW(const uint8_t *p) {
		return
		(((uint16_t)(p[0])      ) |
		 ((uint16_t)(p[1]) <<  8));
	}

	#define U8TO16_LE(p) fU8TO16_LE_SLOW(p)
#endif

#if !defined(U8TO32_LE)
	static INLINE uint32_t
	fU8TO32_LE_SLOW(const uint8_t *p) {
		return
		(((uint32_t)(p[0])      ) |
		 ((uint32_t)(p[1]) <<  8) |
		 ((uint32_t)(p[2]) << 16) |
		 ((uint32_t)(p[3]) << 24));
	}

	#define U8TO32_LE(p) fU8TO32_LE_SLOW(p)
#endif


#if !defined(U8TO64_LE)
	static INLINE uint64_t
	fU8TO64_LE_SLOW(const uint8_t *p) {
		return
		(((uint64_t)(p[0])      ) |
		 ((uint64_t)(p[1]) <<  8) |
		 ((uint64_t)(p[2]) << 16) |
		 ((uint64_t)(p[3]) << 24) |
		 ((uint64_t)(p[4]) << 32) |
		 ((uint64_t)(p[5]) << 40) |
		 ((uint64_t)(p[6]) << 48) |
		 ((uint64_t)(p[7]) << 56));
	}

	#define U8TO64_LE(p) fU8TO64_LE_SLOW(p)
#endif

#if !defined(U16TO8_LE)
	static INLINE void
	fU16TO8_LE_SLOW(uint8_t *p, const uint16_t v) {
		p[0] = (uint8_t)(v      );
		p[1] = (uint8_t)(v >>  8);
	}

	#define U16TO8_LE(p, v) fU16TO8_LE_SLOW(p, v)
#endif

#if !defined(U32TO8_LE)
	static INLINE void
	fU32TO8_LE_SLOW(uint8_t *p, const uint32_t v) {
		p[0] = (uint8_t)(v      );
		p[1] = (uint8_t)(v >>  8);
		p[2] = (uint8_t)(v >> 16);
		p[3] = (uint8_t)(v >> 24);
	}

	#define U32TO8_LE(p, v) fU32TO8_LE_SLOW(p, v)
#endif

#if !defined(U64TO8_LE)
	static INLINE void
	fU64TO8_LE_SLOW(uint8_t *p, const uint64_t v) {
		p[0] = (uint8_t)(v      );
		p[1] = (uint8_t)(v >>  8);
		p[2] = (uint8_t)(v >> 16);
		p[3] = (uint8_t)(v >> 24);
		p[4] = (uint8_t)(v >> 32);
		p[5] = (uint8_t)(v >> 40);
		p[6] = (uint8_t)(v >> 48);
		p[7] = (uint8_t)(v >> 56);
	}

	#define U64TO8_LE(p, v) fU64TO8_LE_SLOW(p, v)
#endif

/* 0400-endian-999-generic-swap.h */

#if !defined(U16_SWAP)
	static INLINE uint16_t
	fU16_SWAP_SLOW(uint16_t v) {
	    v = (v << 8) | (v >> 8);
	    return v;
	}

	#define U16_SWAP(p) fU16_SWAP_SLOW(p)
#endif

#if !defined(U32_SWAP)
	static INLINE uint32_t
	fU32_SWAP_SLOW(uint32_t v) {
		v = ((v << 8) & 0xFF00FF00) | ((v >> 8) & 0xFF00FF);
	    v = (v << 16) | (v >> 16);
	    return v;
	}

	#define U32_SWAP(p) fU32_SWAP_SLOW(p)
#endif

#if !defined(U64_SWAP)
	static INLINE uint64_t
	fU64_SWAP_SLOW(uint64_t v) {
		v = ((v <<  8) & 0xFF00FF00FF00FF00ull) | ((v >>  8) & 0x00FF00FF00FF00FFull);
		v = ((v << 16) & 0xFFFF0000FFFF0000ull) | ((v >> 16) & 0x0000FFFF0000FFFFull);
	    v = (v << 32) | (v >> 32);
	    return v;
	}

	#define U64_SWAP(p) fU64_SWAP_SLOW(p)
#endif

/* 0400-uint128-000.h */

/* 0400-uint128-100-clang.h */

#ifdef HAVE_TI_MODE
# define HAVE_NATIVE_UINT128
typedef unsigned uint128_t __attribute__((mode(TI)));
#endif

/* 0400-uint128-100-msvc.h */

#if defined(CPU_64BITS) && defined(COMPILER_MSVC)
	#define HAVE_UINT128

	typedef struct uint128 {
		uint64_t lo, hi;
	} uint128_t;

	static INLINE uint128_t
	mul64x64_128(uint64_t a, uint64_t b) {
		uint128_t v;
		v.lo = _umul128(a, b, &v.hi);
		return v;
	}

	static INLINE uint64_t
	shr128_pair(uint64_t hi, uint64_t lo, const int shift) {
		return __shiftright128(lo, hi, shift);
	}

	static INLINE uint64_t
	shr128(uint128_t v, const int shift) {
		return __shiftright128(v.lo, v.hi, shift);
	}

	static INLINE uint128_t
	add128(uint128_t a, uint128_t b) {
		uint64_t t = a.lo;
		a.lo += b.lo;
		a.hi += b.hi + (a.lo < t);
		return a;
	}

	static INLINE uint128_t
	add128_64(uint128_t a, uint64_t b) {
		uint64_t t = a.lo;
		a.lo += b;
		a.hi += (a.lo < t);
		return a;
	}

	static INLINE uint64_t
	lo128(uint128_t a) {
		return a.lo;
	}

	static INLINE uint64_t
	hi128(uint128_t a) {
		return a.hi;
	}
#endif

/* 0400-uint128-999.h */

#if defined(HAVE_NATIVE_UINT128)
	#define HAVE_UINT128

	static INLINE uint128_t
	mul64x64_128(uint64_t a, uint64_t b) {
		return (uint128_t)a * b;
	}

	static INLINE uint64_t
	shr128(uint128_t v, const int shift) {
		return (uint64_t)(v >> shift);
	}

	static INLINE uint64_t
	shr128_pair(uint64_t hi, uint64_t lo, const int shift) {
		return (uint64_t)((((uint128_t)hi << 64) | lo) >> shift);
	}

	static INLINE uint128_t
	add128(uint128_t a, uint128_t b) {
		return a + b;
	}

	static INLINE uint128_t
	add128_64(uint128_t a, uint64_t b) {
		return a + b;
	}

	static INLINE uint64_t
	lo128(uint128_t a) {
		return (uint64_t)a;
	}

	static INLINE uint64_t
	hi128(uint128_t a) {
		return (uint64_t)(a >> 64);
	}
#endif

#endif /* PORTABLE_JANE_H */

