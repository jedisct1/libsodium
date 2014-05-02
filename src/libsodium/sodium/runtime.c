
#ifdef HAVE_ANDROID_GETCPUFEATURES
# include <cpu-features.h>
#endif

#include "runtime.h"

typedef struct CPUFeatures_ {
    int initialized;
    int has_neon;
    int has_sse2;
    int has_sse3;
} CPUFeatures;

static CPUFeatures cpu_features;

#define CPUID_SSE2     0x04000000
#define CPUIDECX_SSE3  0x00000001

static int
_sodium_runtime_arm_cpu_features(CPUFeatures * const cpu_features)
{
#ifndef __arm__
    cpu_features->has_neon = 0;
    return -1;
#else
# ifdef __APPLE__
#  ifdef __ARM_NEON__
    cpu_features->has_neon = 1;
#  else
    cpu_features->has_neon = 0;
#  endif
# elif defined(HAVE_ANDROID_GETCPUFEATURES) && defined(ANDROID_CPU_ARM_FEATURE_NEON)
    cpu_features->has_neon =
        (android_getCpuFeatures() & ANDROID_CPU_ARM_FEATURE_NEON) != 0x0;
# else
    cpu_features->has_neon = 0;
# endif
    return 0;
#endif
}

static void
_cpuid(unsigned int cpu_info[4U], const unsigned int cpu_info_type)
{
#ifdef _MSC_VER
    __cpuidex((int *) cpu_info, cpu_info_type, 0);
#elif defined(HAVE_CPUID)
    __asm__ __volatile__ ("cpuid" :
                          "=a" (cpu_info[0]), "=b" (cpu_info[1]),
                          "=c" (cpu_info[2]), "=d" (cpu_info[3]) :
                          "a" (cpu_info_type), "c" (0U));
#else
    cpu_info[0] = cpu_info[1] = cpu_info[2] = cpu_info[3] = 0;
#endif
}

static int
_sodium_runtime_intel_cpu_features(CPUFeatures * const cpu_features)
{
    unsigned int cpu_info[4];
    unsigned int id;

    _cpuid(cpu_info, 0x0);
    if ((id = cpu_info[0]) == 0U) {
        return -1;
    }
    _cpuid(cpu_info, 0x00000001);
    cpu_features->has_sse2 = ((cpu_info[3] & CPUID_SSE2) != 0x0);
    cpu_features->has_sse3 = ((cpu_info[2] & CPUIDECX_SSE3) != 0x0);

    return 0;
}

int
sodium_runtime_get_cpu_features(void)
{
    int ret = -1;

    ret &= _sodium_runtime_arm_cpu_features(&cpu_features);
    ret &= _sodium_runtime_intel_cpu_features(&cpu_features);
    cpu_features.initialized = 1;

    return ret;
}

int
sodium_runtime_has_neon(void) {
    return cpu_features.has_neon;
}

int
sodium_runtime_has_sse2(void) {
    return cpu_features.has_sse2;
}

int
sodium_runtime_has_sse3(void) {
    return cpu_features.has_sse3;
}
