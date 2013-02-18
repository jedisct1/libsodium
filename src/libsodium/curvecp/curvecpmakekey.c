#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "die.h"
#include "e.h"
#include "savesync.h"
#include "randombytes.h"
#include "crypto_box.h"

void die_usage(void)
{
  die_1(111,"curvecpmakekey: usage: curvecpmakekey keydir\n");
}

void die_fatal(const char *trouble,const char *d,const char *fn)
{
  if (fn) die_9(111,"curvecpmakekey: fatal: ",trouble," ",d,"/",fn,": ",e_str(errno),"\n");
  die_7(111,"curvecpmakekey: fatal: ",trouble," ",d,": ",e_str(errno),"\n");
}

unsigned char pk[crypto_box_PUBLICKEYBYTES];
unsigned char sk[crypto_box_SECRETKEYBYTES];
unsigned char lock[1];
unsigned char noncekey[32];
unsigned char noncecounter[8];

void create(const char *d,const char *fn,const unsigned char *x,long long xlen)
{
  if (savesync(fn,x,xlen) == -1) die_fatal("unable to create",d,fn);
}

int main(int argc,char **argv)
{
  char *d;

  if (!argv[0]) die_usage();
  if (!argv[1]) die_usage();
  d = argv[1];

  umask(022);
  if (mkdir(d,0755) == -1) die_fatal("unable to create directory",d,0);
  if (chdir(d) == -1) die_fatal("unable to chdir to directory",d,0);
  if (mkdir(".expertsonly",0700) == -1) die_fatal("unable to create directory",d,".expertsonly");

  crypto_box_keypair(pk,sk);
  create(d,"publickey",pk,sizeof pk);

  randombytes(noncekey,sizeof noncekey);

  umask(077);
  create(d,".expertsonly/secretkey",sk,sizeof sk);
  create(d,".expertsonly/lock",lock,sizeof lock);
  create(d,".expertsonly/noncekey",noncekey,sizeof noncekey);
  create(d,".expertsonly/noncecounter",noncecounter,sizeof noncecounter);

  return 0;
}
