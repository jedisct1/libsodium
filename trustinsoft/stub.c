#include <stdlib.h>
#include <unistd.h>

/* Increase the maximum size of the TIS CI virtual filesystem defined in file
   trustinsoft/common.config */
size_t __mkfs_preallocate_size = 1000000;

int rand(void) {
  return 137;
}

/* Replace the 'SYS_getrandom' system call.
   See 'compilation_cmd' in trustinsoft/gcc_x86_64.config */
int tis_getrandom(void * const buf_, size_t size, int flags)
{
  unsigned char *buf = (unsigned char*) buf_;

  for (size_t i = 0; i < size; i++)
    *buf++ = i;

  return (int)size;
}

/* Stub the call to 'sysconf(_SC_PAGESIZE)'. */
/*@ ensures sysconf_OK: \result != -1; */
long sysconf(int name) {
  if(name == _SC_PAGESIZE) return 4096;
  return -1;
}
