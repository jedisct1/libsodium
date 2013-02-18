#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include "e.h"
#include "die.h"
#include "load.h"
#include "open.h"
#include "byte.h"
#include "socket.h"
#include "uint64_pack.h"
#include "uint64_unpack.h"
#include "nanoseconds.h"
#include "hexparse.h"
#include "nameparse.h"
#include "portparse.h"
#include "writeall.h"
#include "safenonce.h"
#include "randommod.h"

long long recent = 0;

#define NUMIP 8
long long hellowait[NUMIP] = {
   1000000000
,  1500000000
,  2250000000
,  3375000000
,  5062500000
,  7593750000
, 11390625000
, 17085937500
} ;

#include "crypto_box.h"
#include "randombytes.h"
#if crypto_box_PUBLICKEYBYTES != 32
error!
#endif
#if crypto_box_NONCEBYTES != 24
error!
#endif
#if crypto_box_BOXZEROBYTES != 16
error!
#endif
#if crypto_box_ZEROBYTES != 32
error!
#endif
#if crypto_box_BEFORENMBYTES != 32
error!
#endif

int flagverbose = 1;

#define USAGE "\
curvecpclient: how to use:\n\
curvecpclient:   -q (optional): no error messages\n\
curvecpclient:   -Q (optional): print error messages (default)\n\
curvecpclient:   -v (optional): print extra information\n\
curvecpclient:   -c keydir (optional): use this public-key directory\n\
curvecpclient:   sname: server's name\n\
curvecpclient:   pk: server's public key\n\
curvecpclient:   ip: server's IP address\n\
curvecpclient:   port: server's UDP port\n\
curvecpclient:   ext: server's extension\n\
curvecpclient:   prog: run this client\n\
"

void die_usage(const char *s)
{
  if (s) die_4(100,USAGE,"curvecpclient: fatal: ",s,"\n");
  die_1(100,USAGE);
}

void die_fatal(const char *trouble,const char *d,const char *fn)
{
  /* XXX: clean up? OS can do it much more reliably */
  if (!flagverbose) die_0(111);
  if (d) {
    if (fn) die_9(111,"curvecpclient: fatal: ",trouble," ",d,"/",fn,": ",e_str(errno),"\n");
    die_7(111,"curvecpclient: fatal: ",trouble," ",d,": ",e_str(errno),"\n");
  }
  if (errno) die_5(111,"curvecpclient: fatal: ",trouble,": ",e_str(errno),"\n");
  die_3(111,"curvecpclient: fatal: ",trouble,"\n");
}

int multiipparse(unsigned char *y,const char *x)
{
  long long pos;
  long long pos2;
  long long ynum;
  long long ypos;
  long long j;
  long long k;
  long long d;
  for (j = 0;j < 4 * NUMIP;++j) y[j] = 0;
  ynum = 0;
  while (ynum < 1000) {
    ++ynum;
    ypos = randommod(ynum);
    for (k = 0;k < 4;++k) {
      pos = ypos * 4 + k;
      pos2 = (ynum - 1) * 4 + k;
      if (pos >= 0 && pos < 4 * NUMIP && pos2 >= 0 && pos2 < 4 * NUMIP) y[pos2] = y[pos];
      d = 0;
      for (j = 0;j < 3 && x[j] >= '0' && x[j] <= '9';++j) d = d * 10 + (x[j] - '0');
      if (j == 0) return 0;
      x += j;
      if (pos >= 0 && pos < 4 * NUMIP) y[pos] = d;
      if (k < 3) {
        if (*x != '.') return 0;
        ++x;
      }
    }
    if (!*x) break;
    if (*x != ',') return 0;
    ++x;
  }
  /* if fewer than 8 IP addresses, cycle through them: */
  pos = 0;
  pos2 = ynum * 4;
  while (pos2 < 4 * NUMIP) {
    if (pos >= 0 && pos < 4 * NUMIP && pos2 >= 0 && pos2 < 4 * NUMIP) y[pos2] = y[pos];
    ++pos2;
    ++pos;
  }
  return 1;
}


/* routing to the client: */
unsigned char clientextension[16];
long long clientextensionloadtime = 0;
int udpfd = -1;

void clientextension_init(void)
{
  if (recent >= clientextensionloadtime) {
    clientextensionloadtime = recent + 30000000000LL;
    if (load("/etc/curvecpextension",clientextension,16) == -1)
      if (errno == ENOENT || errno == ENAMETOOLONG)
        byte_zero(clientextension,16);
  }
}


/* client security: */
char *keydir = 0;
unsigned char clientlongtermpk[32];
unsigned char clientlongtermsk[32];
unsigned char clientshorttermpk[32];
unsigned char clientshorttermsk[32];
crypto_uint64 clientshorttermnonce;
unsigned char vouch[64];

void clientshorttermnonce_update(void)
{
  ++clientshorttermnonce;
  if (clientshorttermnonce) return;
  errno = EPROTO;
  die_fatal("nonce space expired",0,0);
}

/* routing to the server: */
unsigned char serverip[4 * NUMIP];
unsigned char serverport[2];
unsigned char serverextension[16];

/* server security: */
unsigned char servername[256];
unsigned char serverlongtermpk[32];
unsigned char servershorttermpk[32];
unsigned char servercookie[96];

/* shared secrets: */
unsigned char clientshortserverlong[32];
unsigned char clientshortservershort[32];
unsigned char clientlongserverlong[32];

unsigned char allzero[128] = {0};

unsigned char nonce[24];
unsigned char text[2048];

unsigned char packet[4096];
unsigned char packetip[4];
unsigned char packetport[2];
crypto_uint64 packetnonce;
int flagreceivedmessage = 0;
crypto_uint64 receivednonce = 0;

struct pollfd p[3];

int fdwd = -1;

int tochild[2] = {-1,-1};
int fromchild[2] = {-1,-1};
pid_t child = -1;
int childstatus = 0;

unsigned char childbuf[4096];
long long childbuflen = 0;
unsigned char childmessage[2048];
long long childmessagelen = 0;

int main(int argc,char **argv)
{
  long long hellopackets;
  long long r;
  long long nextaction;

  signal(SIGPIPE,SIG_IGN);

  if (!argv[0]) die_usage(0);
  for (;;) {
    char *x;
    if (!argv[1]) break;
    if (argv[1][0] != '-') break;
    x = *++argv;
    if (x[0] == '-' && x[1] == 0) break;
    if (x[0] == '-' && x[1] == '-' && x[2] == 0) break;
    while (*++x) {
      if (*x == 'q') { flagverbose = 0; continue; }
      if (*x == 'Q') { flagverbose = 1; continue; }
      if (*x == 'v') { if (flagverbose == 2) flagverbose = 3; else flagverbose = 2; continue; }
      if (*x == 'c') {
        if (x[1]) { keydir = x + 1; break; }
        if (argv[1]) { keydir = *++argv; break; }
      }
      die_usage(0);
    }
  }
  if (!nameparse(servername,*++argv)) die_usage("sname must be at most 255 bytes, at most 63 bytes between dots");
  if (!hexparse(serverlongtermpk,32,*++argv)) die_usage("pk must be exactly 64 hex characters");
  if (!multiipparse(serverip,*++argv)) die_usage("ip must be a comma-separated series of IPv4 addresses");
  if (!portparse(serverport,*++argv)) die_usage("port must be an integer between 0 and 65535");
  if (!hexparse(serverextension,16,*++argv)) die_usage("ext must be exactly 32 hex characters");
  if (!*++argv) die_usage("missing prog");

  for (;;) {
    r = open_read("/dev/null");
    if (r == -1) die_fatal("unable to open /dev/null",0,0);
    if (r > 9) { close(r); break; }
  }

  if (keydir) {
    fdwd = open_cwd();
    if (fdwd == -1) die_fatal("unable to open current working directory",0,0);
    if (chdir(keydir) == -1) die_fatal("unable to change to directory",keydir,0);
    if (load("publickey",clientlongtermpk,sizeof clientlongtermpk) == -1) die_fatal("unable to read public key from",keydir,0);
    if (load(".expertsonly/secretkey",clientlongtermsk,sizeof clientlongtermsk) == -1) die_fatal("unable to read secret key from",keydir,0);
  } else {
    crypto_box_keypair(clientlongtermpk,clientlongtermsk);
  }

  crypto_box_keypair(clientshorttermpk,clientshorttermsk);
  clientshorttermnonce = randommod(281474976710656LL);
  crypto_box_beforenm(clientshortserverlong,serverlongtermpk,clientshorttermsk);
  crypto_box_beforenm(clientlongserverlong,serverlongtermpk,clientlongtermsk);

  udpfd = socket_udp();
  if (udpfd == -1) die_fatal("unable to create socket",0,0);

  for (hellopackets = 0;hellopackets < NUMIP;++hellopackets) {
    recent = nanoseconds();

    /* send a Hello packet: */

    clientextension_init();

    clientshorttermnonce_update();
    byte_copy(nonce,16,"CurveCP-client-H");
    uint64_pack(nonce + 16,clientshorttermnonce);

    byte_copy(packet,8,"QvnQ5XlH");
    byte_copy(packet + 8,16,serverextension);
    byte_copy(packet + 24,16,clientextension);
    byte_copy(packet + 40,32,clientshorttermpk);
    byte_copy(packet + 72,64,allzero);
    byte_copy(packet + 136,8,nonce + 16);
    crypto_box_afternm(text,allzero,96,nonce,clientshortserverlong);
    byte_copy(packet + 144,80,text + 16);

    socket_send(udpfd,packet,224,serverip + 4 * hellopackets,serverport);

    nextaction = recent + hellowait[hellopackets] + randommod(hellowait[hellopackets]);

    for (;;) {
      long long timeout = nextaction - recent;
      if (timeout <= 0) break;
      p[0].fd = udpfd;
      p[0].events = POLLIN;
      if (poll(p,1,timeout / 1000000 + 1) < 0) p[0].revents = 0;

      do { /* try receiving a Cookie packet: */
        if (!p[0].revents) break;
        r = socket_recv(udpfd,packet,sizeof packet,packetip,packetport);
        if (r != 200) break;
        if (!(byte_isequal(packetip,4,serverip + 4 * hellopackets) &
              byte_isequal(packetport,2,serverport) &
              byte_isequal(packet,8,"RL3aNMXK") &
              byte_isequal(packet + 8,16,clientextension) &
              byte_isequal(packet + 24,16,serverextension)
           )) break;
        byte_copy(nonce,8,"CurveCPK");
        byte_copy(nonce + 8,16,packet + 40);
        byte_zero(text,16);
        byte_copy(text + 16,144,packet + 56);
        if (crypto_box_open_afternm(text,text,160,nonce,clientshortserverlong)) break;
        byte_copy(servershorttermpk,32,text + 32);
        byte_copy(servercookie,96,text + 64);
        byte_copy(serverip,4,serverip + 4 * hellopackets);
        goto receivedcookie;
      } while (0);

      recent = nanoseconds();
    }
  }

  errno = ETIMEDOUT; die_fatal("no response from server",0,0);

  receivedcookie:

  crypto_box_beforenm(clientshortservershort,servershorttermpk,clientshorttermsk);

  byte_copy(nonce,8,"CurveCPV");
  if (keydir) {
    if (safenonce(nonce + 8,0) == -1) die_fatal("nonce-generation disaster",0,0);
  } else {
    randombytes(nonce + 8,16);
  }

  byte_zero(text,32);
  byte_copy(text + 32,32,clientshorttermpk);
  crypto_box_afternm(text,text,64,nonce,clientlongserverlong);
  byte_copy(vouch,16,nonce + 8);
  byte_copy(vouch + 16,48,text + 16);

  /* server is responding, so start child: */

  if (open_pipe(tochild) == -1) die_fatal("unable to create pipe",0,0);
  if (open_pipe(fromchild) == -1) die_fatal("unable to create pipe",0,0);
  
  child = fork();
  if (child == -1) die_fatal("unable to fork",0,0);
  if (child == 0) {
    if (keydir) if (fchdir(fdwd) == -1) die_fatal("unable to chdir to original directory",0,0);
    close(8);
    if (dup(tochild[0]) != 8) die_fatal("unable to dup",0,0);
    close(9);
    if (dup(fromchild[1]) != 9) die_fatal("unable to dup",0,0);
    /* XXX: set up environment variables */
    signal(SIGPIPE,SIG_DFL);
    execvp(*argv,argv);
    die_fatal("unable to run",*argv,0);
  }

  close(fromchild[1]);
  close(tochild[0]);


  for (;;) {
    p[0].fd = udpfd;
    p[0].events = POLLIN;
    p[1].fd = fromchild[0];
    p[1].events = POLLIN;

    if (poll(p,2,-1) < 0) {
      p[0].revents = 0;
      p[1].revents = 0;
    }

    do { /* try receiving a Message packet: */
      if (!p[0].revents) break;
      r = socket_recv(udpfd,packet,sizeof packet,packetip,packetport);
      if (r < 80) break;
      if (r > 1152) break;
      if (r & 15) break;
      packetnonce = uint64_unpack(packet + 40);
      if (flagreceivedmessage && packetnonce <= receivednonce) break;
      if (!(byte_isequal(packetip,4,serverip + 4 * hellopackets) &
            byte_isequal(packetport,2,serverport) &
            byte_isequal(packet,8,"RL3aNMXM") &
            byte_isequal(packet + 8,16,clientextension) &
            byte_isequal(packet + 24,16,serverextension)
         )) break;
      byte_copy(nonce,16,"CurveCP-server-M");
      byte_copy(nonce + 16,8,packet + 40);
      byte_zero(text,16);
      byte_copy(text + 16,r - 48,packet + 48);
      if (crypto_box_open_afternm(text,text,r - 32,nonce,clientshortservershort)) break;

      if (!flagreceivedmessage) {
        flagreceivedmessage = 1;
	randombytes(clientlongtermpk,sizeof clientlongtermpk);
	randombytes(vouch,sizeof vouch);
	randombytes(servername,sizeof servername);
	randombytes(servercookie,sizeof servercookie);
      }

      receivednonce = packetnonce;
      text[31] = (r - 64) >> 4;
      /* child is responsible for reading all data immediately, so we won't block: */
      if (writeall(tochild[1],text + 31,r - 63) == -1) goto done;
    } while (0);

    do { /* try receiving data from child: */
      long long i;
      if (!p[1].revents) break;
      r = read(fromchild[0],childbuf,sizeof childbuf);
      if (r == -1) if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) break;
      if (r <= 0) goto done;
      childbuflen = r;
      for (i = 0;i < childbuflen;++i) {
	if (childmessagelen < 0) goto done;
	if (childmessagelen >= sizeof childmessage) goto done;
        childmessage[childmessagelen++] = childbuf[i];
	if (childmessage[0] & 128) goto done;
	if (childmessagelen == 1 + 16 * (unsigned long long) childmessage[0]) {
	  clientextension_init();
	  clientshorttermnonce_update();
          uint64_pack(nonce + 16,clientshorttermnonce);
	  if (flagreceivedmessage) {
	    r = childmessagelen - 1;
	    if (r < 16) goto done;
	    if (r > 1088) goto done;
            byte_copy(nonce,16,"CurveCP-client-M");
	    byte_zero(text,32);
	    byte_copy(text + 32,r,childmessage + 1);
	    crypto_box_afternm(text,text,r + 32,nonce,clientshortservershort);
	    byte_copy(packet,8,"QvnQ5XlM");
	    byte_copy(packet + 8,16,serverextension);
	    byte_copy(packet + 24,16,clientextension);
	    byte_copy(packet + 40,32,clientshorttermpk);
	    byte_copy(packet + 72,8,nonce + 16);
	    byte_copy(packet + 80,r + 16,text + 16);
            socket_send(udpfd,packet,r + 96,serverip,serverport);
	  } else {
	    r = childmessagelen - 1;
	    if (r < 16) goto done;
	    if (r > 640) goto done;
	    byte_copy(nonce,16,"CurveCP-client-I");
	    byte_zero(text,32);
	    byte_copy(text + 32,32,clientlongtermpk);
	    byte_copy(text + 64,64,vouch);
	    byte_copy(text + 128,256,servername);
	    byte_copy(text + 384,r,childmessage + 1);
	    crypto_box_afternm(text,text,r + 384,nonce,clientshortservershort);
	    byte_copy(packet,8,"QvnQ5XlI");
	    byte_copy(packet + 8,16,serverextension);
	    byte_copy(packet + 24,16,clientextension);
	    byte_copy(packet + 40,32,clientshorttermpk);
	    byte_copy(packet + 72,96,servercookie);
	    byte_copy(packet + 168,8,nonce + 16);
	    byte_copy(packet + 176,r + 368,text + 16);
            socket_send(udpfd,packet,r + 544,serverip,serverport);
	  }
	  childmessagelen = 0;
	}
      }
    } while (0);
  }


  done:

  do {
    r = waitpid(child,&childstatus,0);
  } while (r == -1 && errno == EINTR);

  if (!WIFEXITED(childstatus)) { errno = 0; die_fatal("process killed by signal",0,0); }
  return WEXITSTATUS(childstatus);
}
