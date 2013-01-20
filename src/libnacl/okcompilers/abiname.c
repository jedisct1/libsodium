#include <stdio.h>

const char *abi(void)
{
#if defined(__amd64__) || defined(__x86_64__) || defined(__AMD64__) || defined(_M_X64) || defined(__amd64)
   return "amd64";
#elif defined(__i386__) || defined(__x86__) || defined(__X86__) || defined(_M_IX86) || defined(__i386)
   return "x86";
#elif defined(__ia64__) || defined(__IA64__) || defined(__M_IA64)
   return "ia64";
#elif defined(__SPU__)
   return "cellspu";
#elif defined(__powerpc64__) || defined(__ppc64__) || defined(__PPC64__) || defined(_ARCH_PPC64)
   return "ppc64";
#elif defined(__powerpc__) || defined(__ppc__) || defined(__PPC__) || defined(_ARCH_PPC)
   return "ppc32";
#elif defined(__sparcv9__) || defined(__sparcv9)
   return "sparcv9";
#elif defined(__sparc_v8__)
   return "sparcv8";
#elif defined(__sparc__) || defined(__sparc)
   if (sizeof(long) == 4) return "sparcv8";
   return "sparcv9";
#elif defined(__ARM_EABI__)
   return "armeabi";
#elif defined(__arm__)
   return "arm";
#elif defined(__mips__) || defined(__mips) || defined(__MIPS__)
#  if defined(_ABIO32)
     return "mipso32";
#  elif defined(_ABIN32)
     return "mips32";
#  else
     return "mips64";
#  endif
#else
   return "default";
#endif
}

int main(int argc,char **argv)
{
  printf("%s %s\n",argv[1],abi());
  return 0;
}
