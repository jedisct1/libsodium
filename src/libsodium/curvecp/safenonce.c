#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "crypto_uint64.h"
#include "uint64_pack.h"
#include "uint64_unpack.h"
#include "savesync.h"
#include "open.h"
#include "load.h"
#include "randombytes.h"
#include "safenonce.h"

#include "crypto_block.h"
#if crypto_block_BYTES != 16
error!
#endif
#if crypto_block_KEYBYTES != 32
error!
#endif

/*
Output: 128-bit nonce y[0],...,y[15].
Reads and writes existing 8-byte file ".expertsonly/noncecounter",
locked via existing 1-byte file ".expertsonly/lock".
Also reads existing 32-byte file ".expertsonly/noncekey".
Not thread-safe.

Invariants:
This process is free to use counters that are >=counterlow and <counterhigh.
The 8-byte file contains a counter that is safe to use and >=counterhigh.

XXX: should rewrite file in background, rather than briefly pausing
*/

static crypto_uint64 counterlow = 0;
static crypto_uint64 counterhigh = 0;

static unsigned char flagkeyloaded = 0;
static unsigned char noncekey[32];
static unsigned char data[16];

int safenonce(unsigned char *y,int flaglongterm)
{
  if (!flagkeyloaded) {
    int fdlock;
    fdlock = open_lock(".expertsonly/lock");
    if (fdlock == -1) return -1;
    if (load(".expertsonly/noncekey",noncekey,sizeof noncekey) == -1) { close(fdlock); return -1; }
    close(fdlock);
    flagkeyloaded = 1;
  }
  
  if (counterlow >= counterhigh) {
    int fdlock;
    fdlock = open_lock(".expertsonly/lock");
    if (fdlock == -1) return -1;
    if (load(".expertsonly/noncecounter",data,8) == -1) { close(fdlock); return -1; }
    counterlow = uint64_unpack(data);
    if (flaglongterm)
      counterhigh = counterlow + 1048576;
    else
      counterhigh = counterlow + 1;
    uint64_pack(data,counterhigh);
    if (savesync(".expertsonly/noncecounter",data,8) == -1) { close(fdlock); return -1; }
    close(fdlock);
  }

  randombytes(data + 8,8);
  uint64_pack(data,counterlow++);
  crypto_block(y,data,noncekey);
  
  return 0;
}
