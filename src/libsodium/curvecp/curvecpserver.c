#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include "e.h"
#include "die.h"
#include "byte.h"
#include "open.h"
#include "load.h"
#include "socket.h"
#include "uint64_pack.h"
#include "uint64_unpack.h"
#include "writeall.h"
#include "nanoseconds.h"
#include "safenonce.h"
#include "nameparse.h"
#include "hexparse.h"
#include "portparse.h"
#include "randommod.h"

#include "randombytes.h"
#include "crypto_box.h"
#include "crypto_secretbox.h"
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
#if crypto_secretbox_KEYBYTES != 32
error!
#endif
#if crypto_secretbox_NONCEBYTES != 24
error!
#endif
#if crypto_secretbox_BOXZEROBYTES != 16
error!
#endif
#if crypto_secretbox_ZEROBYTES != 32
error!
#endif

int flagverbose;

#define USAGE "\
curvecpserver: how to use:\n\
curvecpserver:   -q (optional): no error messages\n\
curvecpserver:   -Q (optional): print error messages (default)\n\
curvecpserver:   -v (optional): print extra information\n\
curvecpserver:   -c n (optional): allow at most n clients at once (default 100)\n\
curvecpserver:   sname: server's name\n\
curvecpserver:   keydir: use this public-key directory\n\
curvecpserver:   ip: server's IP address\n\
curvecpserver:   port: server's UDP port\n\
curvecpserver:   ext: server's extension\n\
curvecpserver:   prog: run this server\n\
"

void die_usage(const char *s)
{
  if (s) die_4(100,USAGE,"curvecpserver: fatal: ",s,"\n");
  die_1(100,USAGE);
}

void die_fatal(const char *trouble,const char *d,const char *fn)
{
  if (!flagverbose) die_0(111);
  if (d) {
    if (fn) die_9(111,"curvecpserver: fatal: ",trouble," ",d,"/",fn,": ",e_str(errno),"\n");
    die_7(111,"curvecpserver: fatal: ",trouble," ",d,": ",e_str(errno),"\n");
  }
  die_5(111,"curvecpserver: fatal: ",trouble,": ",e_str(errno),"\n");
}

int ipparse(unsigned char *y,const char *x)
{
  long long j;
  long long k;
  long long d;

  for (k = 0;k < 4;++k) y[k] = 0;
  for (k = 0;k < 4;++k) {
    d = 0;
    for (j = 0;j < 3 && x[j] >= '0' && x[j] <= '9';++j) d = d * 10 + (x[j] - '0');
    if (j == 0) return 0;
    x += j;
    if (k >= 0 && k < 4) y[k] = d;
    if (k < 3) {
      if (*x != '.') return 0;
      ++x;
    }
  }
  if (*x) return 0;
  return 1;
}

int maxparse(long long *y,const char *x)
{
  long long d;
  long long j;

  d = 0;
  for (j = 0;j < 9 && x[j] >= '0' && x[j] <= '9';++j) d = d * 10 + (x[j] - '0');
  if (x[j]) return 0;
  if (d < 1) return 0;
  if (d > 65535) return 0;
  *y = d;
  return 1;
}

/* cookies: */
long long nextminute;
unsigned char minutekey[32];
unsigned char lastminutekey[32];

/* routing to the server: */
unsigned char serverip[4];
unsigned char serverport[2];
unsigned char serverextension[16];
int udpfd = -1;

/* server security: */
char *keydir = 0;
unsigned char servername[256];
unsigned char serverlongtermsk[32];
unsigned char servershorttermpk[32];
unsigned char servershorttermsk[32];

/* routing to the client: */
unsigned char clientextension[16];

/* client security: */
unsigned char clientlongtermpk[32];
unsigned char clientshorttermpk[32];

/* shared secrets: */
unsigned char clientshortserverlong[32];
unsigned char clientshortservershort[32];
unsigned char clientlongserverlong[32];

unsigned char allzero[128] = {0};

unsigned char nonce[24];
unsigned char text[2048];

unsigned char packetip[4];
unsigned char packetport[2];
unsigned char packet[4096];
crypto_uint64 packetnonce;

#define MESSAGELEN 1104

struct activeclient {
  unsigned char clientshorttermpk[32];
  unsigned char clientshortservershort[32];
  crypto_uint64 receivednonce;
  crypto_uint64 sentnonce;
  long long messagelen;
  pid_t child;
  int tochild;
  int fromchild;
  unsigned char clientextension[16];
  unsigned char clientip[4];
  unsigned char clientport[2];
  unsigned char message[MESSAGELEN];
} ;

const char *strmaxactiveclients = "100";
long long maxactiveclients = 0;
long long numactiveclients = 0;
struct activeclient *activeclients = 0;
struct pollfd *p;

int fdwd = -1;

int pi0[2];
int pi1[2];

unsigned char childbuf[4096];
long long childbuflen = 0;
unsigned char childmessage[2048];
long long childmessagelen = 0;

int main(int argc,char **argv)
{
  long long r;
  long long i;
  long long k;

  signal(SIGPIPE,SIG_IGN);
  signal(SIGCHLD,SIG_IGN);

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
        if (x[1]) { strmaxactiveclients = x + 1; break; }
	if (argv[1]) { strmaxactiveclients = *++argv; break; }
      }
      die_usage(0);
    }
  }
  if (!maxparse(&maxactiveclients,strmaxactiveclients)) die_usage("concurrency must be between 1 and 65535");
  if (!nameparse(servername,*++argv)) die_usage("sname must be at most 255 bytes, at most 63 bytes between dots");
  keydir = *++argv; if (!keydir) die_usage("missing keydir");
  if (!ipparse(serverip,*++argv)) die_usage("ip must be an IPv4 address");
  if (!portparse(serverport,*++argv)) die_usage("port must be an integer between 0 and 65535");
  if (!hexparse(serverextension,16,*++argv)) die_usage("ext must be exactly 32 hex characters");
  if (!*++argv) die_usage("missing prog");

  for (;;) {
    r = open_read("/dev/null");
    if (r == -1) die_fatal("unable to open /dev/null",0,0);
    if (r > 9) { close(r); break; }
  }

  activeclients = malloc(maxactiveclients * sizeof(struct activeclient));
  if (!activeclients) die_fatal("unable to create activeclients array",0,0);
  randombytes((void *) activeclients,maxactiveclients * sizeof(struct activeclient));
  for (i = 0;i < maxactiveclients;++i) {
    activeclients[i].child = -1;
    activeclients[i].tochild = -1;
    activeclients[i].fromchild = -1;
    activeclients[i].receivednonce = 0;
    activeclients[i].sentnonce = randommod(281474976710656LL);
  }
  
  p = malloc((1 + maxactiveclients) * sizeof(struct pollfd));
  if (!p) die_fatal("unable to create poll array",0,0);

  fdwd = open_cwd();
  if (fdwd == -1) die_fatal("unable to open current directory",0,0);

  if (chdir(keydir) == -1) die_fatal("unable to chdir to",keydir,0);
  if (load(".expertsonly/secretkey",serverlongtermsk,sizeof serverlongtermsk) == -1) die_fatal("unable to read secret key from",keydir,0);

  udpfd = socket_udp();
  if (udpfd == -1) die_fatal("unable to create socket",0,0);
  if (socket_bind(udpfd,serverip,serverport) == -1) die_fatal("unable to bind socket",0,0);

  randombytes(minutekey,sizeof minutekey);
  randombytes(lastminutekey,sizeof lastminutekey);
  nextminute = nanoseconds() + 60000000000ULL;

  for (;;) {
    long long timeout = nextminute - nanoseconds();
    if (timeout <= 0) {
      timeout = 60000000000ULL;
      byte_copy(lastminutekey,sizeof lastminutekey,minutekey);
      randombytes(minutekey,sizeof minutekey);
      nextminute = nanoseconds() + timeout;
      randombytes(packet,sizeof packet);
      randombytes(packetip,sizeof packetip);
      randombytes(packetport,sizeof packetport);
      randombytes(clientshorttermpk,sizeof clientshorttermpk);
      randombytes(clientshortserverlong,sizeof clientshortserverlong);
      randombytes(nonce,sizeof nonce);
      randombytes(text,sizeof text);
      randombytes(childbuf,sizeof childbuf);
      randombytes(childmessage,sizeof childmessage);
      randombytes(servershorttermpk,sizeof servershorttermpk);
      randombytes(servershorttermsk,sizeof servershorttermsk);
    }

    for (i = 0;i < numactiveclients;++i) {
      p[i].fd = activeclients[i].fromchild;
      p[i].events = POLLIN;
    }
    p[numactiveclients].fd = udpfd;
    p[numactiveclients].events = POLLIN;
    if (poll(p,1 + numactiveclients,timeout / 1000000 + 1) < 0) continue;

    do { /* try receiving a packet: */
      if (!p[numactiveclients].revents) break;
      r = socket_recv(udpfd,packet,sizeof packet,packetip,packetport);
      if (r < 80) break;
      if (r > 1184) break;
      if (r & 15) break;
      if (!(byte_isequal(packet,7,"QvnQ5Xl") & byte_isequal(packet + 8,16,serverextension))) break;
      byte_copy(clientextension,16,packet + 24);
      if (packet[7] == 'H') { /* Hello packet: */
        if (r != 224) break;
	byte_copy(clientshorttermpk,32,packet + 40);
	crypto_box_beforenm(clientshortserverlong,clientshorttermpk,serverlongtermsk);
	byte_copy(nonce,16,"CurveCP-client-H");
	byte_copy(nonce + 16,8,packet + 136);
	byte_zero(text,16);
	byte_copy(text + 16,80,packet + 144);
	if (crypto_box_open_afternm(text,text,96,nonce,clientshortserverlong)) break;

	/* send Cookie packet: */

	crypto_box_keypair(servershorttermpk,servershorttermsk);
	byte_zero(text + 64,32);
	byte_copy(text + 96,32,clientshorttermpk);
	byte_copy(text + 128,32,servershorttermsk);
	byte_copy(nonce,8,"minute-k");
	if (safenonce(nonce + 8,1) == -1) die_fatal("nonce-generation disaster",0,0);
	crypto_secretbox(text + 64,text + 64,96,nonce,minutekey);
	byte_copy(text + 64,16,nonce + 8);

	byte_zero(text,32);
	byte_copy(text + 32,32,servershorttermpk);
	byte_copy(nonce,8,"CurveCPK"); /* reusing the other 16 bytes */
	crypto_box_afternm(text,text,160,nonce,clientshortserverlong);

	byte_copy(packet,8,"RL3aNMXK");
	byte_copy(packet + 8,16,clientextension);
	byte_copy(packet + 24,16,serverextension);
	byte_copy(packet + 40,16,nonce + 8);
	byte_copy(packet + 56,144,text + 16);

	socket_send(udpfd,packet,200,packetip,packetport);
      }
      if (packet[7] == 'I') { /* Initiate packet: */
        if (r < 560) break;
	for (i = 0;i < numactiveclients;++i) /* XXX use better data structure */
	  if (byte_isequal(activeclients[i].clientshorttermpk,32,packet + 40))
	    break;
	if (i < numactiveclients) {
	  packetnonce = uint64_unpack(packet + 168);
	  if (packetnonce <= activeclients[i].receivednonce) break;
	  byte_copy(nonce,16,"CurveCP-client-I");
	  byte_copy(nonce + 16,8,packet + 168);
	  byte_zero(text,16);
  	  byte_copy(text + 16,r - 176,packet + 176);
	  if (crypto_box_open_afternm(text,text,r - 160,nonce,activeclients[i].clientshortservershort)) break;

	  /* XXX: update clientip, clientextension; but not if client has spoken recently */
	  activeclients[i].receivednonce = packetnonce;
	  text[383] = (r - 544) >> 4;
	  if (writeall(activeclients[i].tochild,text + 383,r - 543) == -1)
	    ; /* child is gone; will see eof later */
	  break;
	}
	if (i == maxactiveclients) break;

	byte_copy(nonce,8,"minute-k");
	byte_copy(nonce + 8,16,packet + 72);
	byte_zero(text,16);
	byte_copy(text + 16,80,packet + 88);
	if (crypto_secretbox_open(text,text,96,nonce,minutekey)) {
	  byte_zero(text,16);
	  byte_copy(text + 16,80,packet + 88);
	  if (crypto_secretbox_open(text,text,96,nonce,lastminutekey)) break;
	}
	if (!byte_isequal(packet + 40,32,text + 32)) break;
	byte_copy(servershorttermsk,32,text + 64);
	byte_copy(clientshorttermpk,32,packet + 40);
	crypto_box_beforenm(clientshortservershort,clientshorttermpk,servershorttermsk);

	byte_copy(nonce,16,"CurveCP-client-I");
	byte_copy(nonce + 16,8,packet + 168);
	byte_zero(text,16);
	byte_copy(text + 16,r - 176,packet + 176);
	if (crypto_box_open_afternm(text,text,r - 160,nonce,clientshortservershort)) break;

	if (!byte_isequal(text + 128,256,servername)) break;

	/* XXX skip if client authentication is not desired: */
	byte_copy(clientlongtermpk,32,text + 32);
	/* XXX impose policy limitations on clients: known, maxconn */
	/* XXX for known clients, retrieve shared secret from cache: */
	crypto_box_beforenm(clientlongserverlong,clientlongtermpk,serverlongtermsk);
	byte_copy(nonce,8,"CurveCPV");
	byte_copy(nonce + 8,16,text + 64);
	byte_zero(text + 64,16);
	if (crypto_box_open_afternm(text + 64,text + 64,64,nonce,clientlongserverlong)) break;
	if (!byte_isequal(text + 96,32,clientshorttermpk)) break;

	if (open_pipe(pi0) == -1) break; /* XXX: error message */
	if (open_pipe(pi1) == -1) { close(pi0[0]); close(pi0[1]); break; } /* XXX: error message */

	activeclients[i].child = fork();
	if (activeclients[i].child == -1) {
	  close(pi0[0]); close(pi0[1]);
	  close(pi1[0]); close(pi1[1]);
	  break; /* XXX: error message */
	}
	if (activeclients[i].child == 0) {
	  if (fchdir(fdwd) == -1) die_fatal("unable to chdir to original directory",0,0);
	  close(8);
	  if (dup(pi0[0]) != 8) die_fatal("unable to dup",0,0);
	  close(9);
	  if (dup(pi1[1]) != 9) die_fatal("unable to dup",0,0);
	  /* XXX: set up environment variables */
	  signal(SIGPIPE,SIG_DFL);
	  signal(SIGCHLD,SIG_DFL);
	  execvp(*argv,argv);
	  die_fatal("unable to run",*argv,0);
	}

	activeclients[i].tochild = pi0[1]; close(pi0[0]);
	activeclients[i].fromchild = pi1[0]; close(pi1[1]);
	activeclients[i].messagelen = 0;
	byte_copy(activeclients[i].clientshorttermpk,32,clientshorttermpk);
	byte_copy(activeclients[i].clientshortservershort,32,clientshortservershort);
	activeclients[i].receivednonce = uint64_unpack(packet + 168);
	byte_copy(activeclients[i].clientextension,16,clientextension);
	byte_copy(activeclients[i].clientip,4,packetip);
	byte_copy(activeclients[i].clientport,2,packetport);
	++numactiveclients;

	text[383] = (r - 544) >> 4;
	if (writeall(activeclients[i].tochild,text + 383,r - 543) == -1)
	  ; /* child is gone; will see eof later */
      }
      if (packet[7] == 'M') { /* Message packet: */
        if (r < 112) break;
        for (i = 0;i < numactiveclients;++i) /* XXX use better data structure */
	  if (byte_isequal(activeclients[i].clientshorttermpk,32,packet + 40))
	    break;
	if (i < numactiveclients) {
	  packetnonce = uint64_unpack(packet + 72);
	  if (packetnonce <= activeclients[i].receivednonce) break;
          byte_copy(nonce,16,"CurveCP-client-M");
	  byte_copy(nonce + 16,8,packet + 72);
	  byte_zero(text,16);
	  byte_copy(text + 16,r - 80,packet + 80);
	  if (crypto_box_open_afternm(text,text,r - 64,nonce,activeclients[i].clientshortservershort)) break;

	  /* XXX: update clientip, clientextension */
	  activeclients[i].receivednonce = packetnonce;
	  text[31] = (r - 96) >> 4;
	  if (writeall(activeclients[i].tochild,text + 31,r - 95) == -1)
	    ; /* child is gone; will see eof later */
	  break;
	}
      }
    } while (0);

    for (i = numactiveclients - 1;i >= 0;--i) {
      do {
        if (!p[i].revents) break;
	r = read(activeclients[i].fromchild,childbuf,sizeof childbuf);
	if (r == -1) if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) break;
	if (r <= 0) goto endconnection;
	childbuflen = r;
	for (k = 0;k < childbuflen;++k) {
	  r = activeclients[i].messagelen;
	  if (r < 0) goto endconnection;
	  if (r >= MESSAGELEN) goto endconnection;
	  activeclients[i].message[r] = childbuf[k];
	  if (r == 0) if (childbuf[k] & 128) goto endconnection;
	  activeclients[i].messagelen = r + 1;
	  if (r == 16 * (unsigned long long) activeclients[i].message[0]) {
	    if (r < 16) goto endconnection;
	    if (r > 1088) goto endconnection;
	    byte_copy(nonce,16,"CurveCP-server-M");
	    uint64_pack(nonce + 16,++activeclients[i].sentnonce);
	    byte_zero(text,32);
	    byte_copy(text + 32,r,activeclients[i].message + 1);
	    crypto_box_afternm(text,text,r + 32,nonce,activeclients[i].clientshortservershort);
	    byte_copy(packet,8,"RL3aNMXM");
	    byte_copy(packet + 8,16,clientextension);
	    byte_copy(packet + 24,16,serverextension);
	    byte_copy(packet + 40,8,nonce + 16);
	    byte_copy(packet + 48,r + 16,text + 16);
	    socket_send(udpfd,packet,r + 64,activeclients[i].clientip,activeclients[i].clientport);
	    activeclients[i].messagelen = 0;
	  }
	}
	break;

	endconnection:

	/* XXX: cache cookie if it's recent */
	close(activeclients[i].fromchild); activeclients[i].fromchild = -1;
	close(activeclients[i].tochild); activeclients[i].tochild = -1;
	--numactiveclients;
	activeclients[i] = activeclients[numactiveclients];
	randombytes((void *) &activeclients[numactiveclients],sizeof(struct activeclient));
      } while (0);
    }
  }
}
