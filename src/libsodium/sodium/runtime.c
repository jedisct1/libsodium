#include <stddef.h>
#include <stdint.h>
#ifdef HAVE_ANDROID_GETCPUFEATURES
# include <cpu-features.h>
#endif
#ifdef __APPLE__
# include <sys/types.h>
# include <sys/sysctl.h>
# include <mach/machine.h>
#endif
#ifdef HAVE_SYS_AUXV_H
# include <sys/auxv.h>
#endif

#include "private/common.h"
#include "runtime.h"

typedef struct CPUFeatures_ {
    int initialized;
    int has_neon;
    int has_armcrypto;
    int has_sse2;
    int has_sse3;
    int has_ssse3;
    int has_sse41;
    int has_avx;
    int has_avx2;
    int has_avx512f;
    int has_pclmul;
    int has_aesni;
    int has_rdrand;
} CPUFeatures;

static CPUFeatures _cpu_features;

#define CPUID_EBX_AVX2    0x00000020
#define CPUID_EBX_AVX512F 0x00010000

#define CPUID_ECX_SSE3    0x00000001
#define CPUID_ECX_PCLMUL  0x00000002
#define CPUID_ECX_SSSE3   0x00000200
#define CPUID_ECX_SSE41   0x00080000
#define CPUID_ECX_AESNI   0x02000000
#define CPUID_ECX_XSAVE   0x04000000
#define CPUID_ECX_OSXSAVE 0x08000000
#define CPUID_ECX_AVX     0x10000000
#define CPUID_ECX_RDRAND  0x40000000

#define CPUID_EDX_SSE2    0x04000000

#define XCR0_SSE       0x00000002
#define XCR0_AVX       0x00000004
#define XCR0_OPMASK    0x00000020
#define XCR0_ZMM_HI256 0x00000040
#define XCR0_HI16_ZMM  0x00000080

static int
_sodium_runtime_arm_cpu_features(CPUFeatures * const cpu_features)
{
    cpu_features->has_neon = 0;
    cpu_features->has_armcrypto = 0;

#ifndef __ARM_ARCH
    return -1; /* LCOV_EXCL_LINE */
#endif

#if defined(__ARM_NEON) || defined(__aarch64__)
    cpu_features->has_neon = 1;
#elif defined(HAVE_ANDROID_GETCPUFEATURES) && defined(ANDROID_CPU_ARM_FEATURE_NEON)
    cpu_features->has_neon =
        (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) != 0x0;
#elif defined(__aarch64__) && defined(AT_HWCAP)
# ifdef HAVE_GETAUXVAL
    cpu_features->has_neon = (getauxval(AT_HWCAP) & (1L << 1)) != 0;
# elif defined(HAVE_ELF_AUX_INFO)
    {
        unsigned long buf;
        if (elf_aux_info(AT_HWCAP, (void *) &buf, (int) sizeof buf) == 0) {
            cpu_features->has_neon = (buf & (1L << 1)) != 0;
        }
    }
# endif
#elif defined(__arm__) && defined(AT_HWCAP)
# ifdef HAVE_GETAUXVAL
    cpu_features->has_neon = (getauxval(AT_HWCAP) & (1L << 12)) != 0;
# elif defined(HAVE_ELF_AUX_INFO)
    {
        unsigned long buf;
        if (elf_aux_info(AT_HWCAP, (void *) &buf, (int) sizeof buf) == 0) {
            cpu_features->has_neon = (buf & (1L << 12)) != 0;
        }
    }
# endif
#endif

    if (cpu_features->has_neon == 0) {
        return 0;
    }

#if __ARM_FEATURE_CRYPTO
    cpu_features->has_armcrypto = 1;
#elif defined(__APPLE__) && defined(CPU_TYPE_ARM64) && defined(CPU_SUBTYPE_ARM64E)
    {
        cpu_type_t    cpu_type;
        cpu_subtype_t cpu_subtype;
        size_t        cpu_type_len = sizeof cpu_type;
        size_t        cpu_subtype_len = sizeof cpu_subtype;

        if (sysctlbyname("hw.cputype", &cpu_type, &cpu_type_len,
                         NULL, 0) == 0 && cpu_type == CPU_TYPE_ARM64 &&
            sysctlbyname("hw.cpusubtype", &cpu_subtype, &cpu_subtype_len,
                         NULL, 0) == 0 &&
            (cpu_subtype == CPU_SUBTYPE_ARM64E ||
                cpu_subtype == CPU_SUBTYPE_ARM64_V8)) {
            cpu_features->has_armcrypto = 1;
        }
    }
#elif defined(HAVE_ANDROID_GETCPUFEATURES) && defined(ANDROID_CPU_ARM_FEATURE_AES)
    cpu_features->has_armcrypto =
        (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_AES) != 0x0;
#elif defined(__aarch64__) && defined(AT_HWCAP)
# ifdef HAVE_GETAUXVAL
    cpu_features->has_armcrypto = (getauxval(AT_HWCAP) & (1L << 3)) != 0;
# elif defined(HAVE_ELF_AUX_INFO)
    {
        unsigned long buf;
        if (elf_aux_info(AT_HWCAP, (void *) &buf, (int) sizeof buf) == 0) {
            cpu_features->has_armcrypto = (buf & (1L << 3)) != 0;
        }
    }
# endif
#elif defined(__arm__) && defined(AT_HWCAP2)
# ifdef HAVE_GETAUXVAL
    cpu_features->has_armcrypto = (getauxval(AT_HWCAP2) & (1L << 0)) != 0;
# elif defined(HAVE_ELF_AUX_INFO)
    {
        unsigned long buf;
        if (elf_aux_info(AT_HWCAP2, (void *) &buf, (int) sizeof buf) == 0) {
            cpu_features->has_armcrypto = (buf & (1L << 0)) != 0;
        }
    }
# endif
#endif

    return 0;
}

static void
_cpuid(unsigned int cpu_info[4U], const unsigned int cpu_info_type)
{
#if defined(_MSC_VER) && \
    (defined(_M_X64) || defined(_M_AMD64) || defined(_M_IX86))
    __cpuid((int *) cpu_info, cpu_info_type);
#elif defined(HAVE_CPUID)
    cpu_info[0] = cpu_info[1] = cpu_info[2] = cpu_info[3] = 0;
# ifdef __i386__
    __asm__ __volatile__(
        "pushfl; pushfl; "
        "popl %0; "
        "movl %0, %1; xorl %2, %0; "
        "pushl %0; "
        "popfl; pushfl; popl %0; popfl"
        : "=&r"(cpu_info[0]), "=&r"(cpu_info[1])
        : "i"(0x200000));
    if (((cpu_info[0] ^ cpu_info[1]) & 0x200000) == 0x0) {
        return; /* LCOV_EXCL_LINE */
    }
# endif
# ifdef __i386__
    __asm__ __volatile__("xchgl %%ebx, %k1; cpuid; xchgl %%ebx, %k1"
                         : "=a"(cpu_info[0]), "=&r"(cpu_info[1]),
                           "=c"(cpu_info[2]), "=d"(cpu_info[3])
                         : "0"(cpu_info_type), "2"(0U));
# elif defined(__x86_64__)
    __asm__ __volatile__("xchgq %%rbx, %q1; cpuid; xchgq %%rbx, %q1"
                         : "=a"(cpu_info[0]), "=&r"(cpu_info[1]),
                           "=c"(cpu_info[2]), "=d"(cpu_info[3])
                         : "0"(cpu_info_type), "2"(0U));
# else
    __asm__ __volatile__("cpuid"
                         : "=a"(cpu_info[0]), "=b"(cpu_info[1]),
                           "=c"(cpu_info[2]), "=d"(cpu_info[3])
                         : "0"(cpu_info_type), "2"(0U));
# endif
#else
    (void) cpu_info_type;
    cpu_info[0] = cpu_info[1] = cpu_info[2] = cpu_info[3] = 0;
#endif
}

static int
_sodium_runtime_intel_cpu_features(CPUFeatures * const cpu_features)
{
    unsigned int cpu_info[4];
    uint32_t     xcr0 = 0U;

    _cpuid(cpu_info, 0x0);
    if (cpu_info[0] == 0U) {
        return -1; /* LCOV_EXCL_LINE */
    }
    _cpuid(cpu_info, 0x00000001);
#ifdef HAVE_EMMINTRIN_H
    cpu_features->has_sse2 = ((cpu_info[3] & CPUID_EDX_SSE2) != 0x0);
#else
    cpu_features->has_sse2   = 0;
#endif

#ifdef HAVE_PMMINTRIN_H
    cpu_features->has_sse3 = ((cpu_info[2] & CPUID_ECX_SSE3) != 0x0);
#else
    cpu_features->has_sse3   = 0;
#endif

#ifdef HAVE_TMMINTRIN_H
    cpu_features->has_ssse3 = ((cpu_info[2] & CPUID_ECX_SSSE3) != 0x0);
#else
    cpu_features->has_ssse3  = 0;
#endif

#ifdef HAVE_SMMINTRIN_H
    cpu_features->has_sse41 = ((cpu_info[2] & CPUID_ECX_SSE41) != 0x0);
#else
    cpu_features->has_sse41  = 0;
#endif

    cpu_features->has_avx = 0;

    (void) xcr0;
#ifdef HAVE_AVXINTRIN_H
    if ((cpu_info[2] & (CPUID_ECX_AVX | CPUID_ECX_XSAVE | CPUID_ECX_OSXSAVE)) ==
        (CPUID_ECX_AVX | CPUID_ECX_XSAVE | CPUID_ECX_OSXSAVE)) {
        xcr0 = 0U;
# if defined(HAVE__XGETBV) || \
        (defined(_MSC_VER) && defined(_XCR_XFEATURE_ENABLED_MASK) && _MSC_FULL_VER >= 160040219)
        xcr0 = (uint32_t) _xgetbv(0);
# elif defined(_MSC_VER) && defined(_M_IX86)
        /*
         * Visual Studio documentation states that eax/ecx/edx don't need to
         * be preserved in inline assembly code. But that doesn't seem to
         * always hold true on Visual Studio 2010.
         */
        __asm {
            push eax
            push ecx
            push edx
            xor ecx, ecx
            _asm _emit 0x0f _asm _emit 0x01 _asm _emit 0xd0
            mov xcr0, eax
            pop edx
            pop ecx
            pop eax
        }
# elif defined(HAVE_AVX_ASM)
        __asm__ __volatile__(".byte 0x0f, 0x01, 0xd0" /* XGETBV */
                             : "=a"(xcr0)
                             : "c"((uint32_t) 0U)
                             : "%edx");
# endif
        if ((xcr0 & (XCR0_SSE | XCR0_AVX)) == (XCR0_SSE | XCR0_AVX)) {
            cpu_features->has_avx = 1;
        }
    }
#endif

    cpu_features->has_avx2 = 0;
#ifdef HAVE_AVX2INTRIN_H
    if (cpu_features->has_avx) {
        unsigned int cpu_info7[4];

        _cpuid(cpu_info7, 0x00000007);
        cpu_features->has_avx2 = ((cpu_info7[1] & CPUID_EBX_AVX2) != 0x0);
    }
#endif

    cpu_features->has_avx512f = 0;
#ifdef HAVE_AVX512FINTRIN_H
    if (cpu_features->has_avx2) {
        unsigned int cpu_info7[4];

        _cpuid(cpu_info7, 0x00000007);
        /* LCOV_EXCL_START */
        if ((cpu_info7[1] & CPUID_EBX_AVX512F) == CPUID_EBX_AVX512F &&
            (xcr0 & (XCR0_OPMASK | XCR0_ZMM_HI256 | XCR0_HI16_ZMM))
            == (XCR0_OPMASK | XCR0_ZMM_HI256 | XCR0_HI16_ZMM)) {
            cpu_features->has_avx512f = 1;
        }
        /* LCOV_EXCL_STOP */
    }
#endif

#ifdef HAVE_WMMINTRIN_H
    cpu_features->has_pclmul = ((cpu_info[2] & CPUID_ECX_PCLMUL) != 0x0);
    cpu_features->has_aesni  = ((cpu_info[2] & CPUID_ECX_AESNI) != 0x0);
#else
    cpu_features->has_pclmul = 0;
    cpu_features->has_aesni  = 0;
#endif

#ifdef HAVE_RDRAND
    cpu_features->has_rdrand = ((cpu_info[2] & CPUID_ECX_RDRAND) != 0x0);
#else
    cpu_features->has_rdrand = 0;
#endif

    return 0;
}

int
_sodium_runtime_get_cpu_features(void)
{
    int ret = -1;

    ret &= _sodium_runtime_arm_cpu_features(&_cpu_features);
    ret &= _sodium_runtime_intel_cpu_features(&_cpu_features);
    _cpu_features.initialized = 1;

    return ret;
}

int
sodium_runtime_has_neon(void)
{
    return _cpu_features.has_neon;
}

int
sodium_runtime_has_armcrypto(void)
{
    return _cpu_features.has_armcrypto;
}

int
sodium_runtime_has_sse2(void)
{
    return _cpu_features.has_sse2;
}

int
sodium_runtime_has_sse3(void)
{
    return _cpu_features.has_sse3;
}

int
sodium_runtime_has_ssse3(void)
{
    return _cpu_features.has_ssse3;
}

int
sodium_runtime_has_sse41(void)
{
    return _cpu_features.has_sse41;
}

int
sodium_runtime_has_avx(void)
{
    return _cpu_features.has_avx;
}

int
sodium_runtime_has_avx2(void)
{
    return _cpu_features.has_avx2;
}

int
sodium_runtime_has_avx512f(void)
{
    return _cpu_features.has_avx512f;
}

int
sodium_runtime_has_pclmul(void)
{
    return _cpu_features.has_pclmul;
}

int
sodium_runtime_has_aesni(void)
{
    return _cpu_features.has_aesni;
}

int
sodium_runtime_has_rdrand(void)
{
    return _cpu_features.has_rdrand;
}
