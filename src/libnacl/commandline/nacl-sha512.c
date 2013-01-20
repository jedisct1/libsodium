/*
commandline/nacl-sha512.c version 20080713
D. J. Bernstein
Public domain.
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "crypto_hash_sha512.h"

unsigned char *input;
unsigned long long inputalloc;
unsigned long long inputlen;

unsigned char h[crypto_hash_sha512_BYTES];

void h_print(void)
{
  int i;
  for (i = 0;i < crypto_hash_sha512_BYTES;++i) printf("%02x",255 & (int) h[i]);
  printf("\n");
}

int main()
{
  struct stat st;
  int ch;

  if (fstat(0,&st) == 0) {
    input = mmap(0,st.st_size,PROT_READ,MAP_SHARED,0,0);
    if (input != MAP_FAILED) {
      crypto_hash_sha512(h,input,st.st_size);
      h_print();
      return 0;
    }
  }

  input = 0;
  inputalloc = 0;
  inputlen = 0;

  while ((ch = getchar()) != EOF) {
    if (inputlen >= inputalloc) {
      void *newinput;
      while (inputlen >= inputalloc)
        inputalloc = inputalloc * 2 + 1;
      if (posix_memalign(&newinput,16,inputalloc) != 0) return 111;
      memcpy(newinput,input,inputlen);
      free(input);
      input = newinput;
    }
    input[inputlen++] = ch;
  }

  crypto_hash_sha512(h,input,inputlen);
  h_print();

  return 0;
}
