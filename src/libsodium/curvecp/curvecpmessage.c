#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include "open.h"
#include "blocking.h"
#include "e.h"
#include "die.h"
#include "randommod.h"
#include "byte.h"
#include "crypto_uint32.h"
#include "uint16_pack.h"
#include "uint32_pack.h"
#include "uint64_pack.h"
#include "uint16_unpack.h"
#include "uint32_unpack.h"
#include "uint64_unpack.h"
#include "nanoseconds.h"
#include "writeall.h"

int flagverbose = 1;
int flagserver = 1;
int wantping = 0; /* 1: ping after a second; 2: ping immediately */

#define USAGE "\
curvecpmessage: how to use:\n\
curvecpmessage:   -q (optional): no error messages\n\
curvecpmessage:   -Q (optional): print error messages (default)\n\
curvecpmessage:   -v (optional): print extra information\n\
curvecpmessage:   -c (optional): program is a client; server starts first\n\
curvecpmessage:   -C (optional): program is a client that starts first\n\
curvecpmessage:   -s (optional): program is a server (default)\n\
curvecpmessage:   prog: run this program\n\
"

void die_usage(const char *s)
{
  if (s) die_4(100,USAGE,"curvecpmessage: fatal: ",s,"\n");
  die_1(100,USAGE);
}

void die_fatal(const char *trouble,const char *d,const char *fn)
{
  if (!flagverbose) die_0(111);
  if (d) {
    if (fn) die_9(111,"curvecpmessage: fatal: ",trouble," ",d,"/",fn,": ",e_str(errno),"\n");
    die_7(111,"curvecpmessage: fatal: ",trouble," ",d,": ",e_str(errno),"\n");
  }
  if (errno) die_5(111,"curvecpmessage: fatal: ",trouble,": ",e_str(errno),"\n");
  die_3(111,"curvecpmessage: fatal: ",trouble,"\n");
}

void die_badmessage(void)
{
  errno = EPROTO;
  die_fatal("unable to read from file descriptor 8",0,0);
}

void die_internalerror(void)
{
  errno = EPROTO;
  die_fatal("internal error",0,0);
}


int tochild[2] = {-1,-1};
int fromchild[2] = {-1,-1};
pid_t child = -1;
int childstatus;

struct pollfd p[3];

long long sendacked = 0; /* number of initial bytes sent and fully acknowledged */
long long sendbytes = 0; /* number of additional bytes to send */
unsigned char sendbuf[131072]; /* circular queue with the additional bytes; size must be power of 2 */
long long sendprocessed = 0; /* within sendbytes, number of bytes absorbed into blocks */

crypto_uint16 sendeof = 0; /* 2048 for normal eof after sendbytes, 4096 for error after sendbytes */
int sendeofprocessed = 0;
int sendeofacked = 0;

long long totalblocktransmissions = 0;
long long totalblocks = 0;

#define OUTGOING 128 /* must be power of 2 */
long long blocknum = 0; /* number of outgoing blocks being tracked */
long long blockfirst = 0; /* circular queue */
long long blockpos[OUTGOING]; /* position of block's first byte within stream */
long long blocklen[OUTGOING]; /* number of bytes in this block */
crypto_uint16 blockeof[OUTGOING]; /* 0, 2048, 4096 */
long long blocktransmissions[OUTGOING];
long long blocktime[OUTGOING]; /* time of last message sending this block; 0 means acked */
long long earliestblocktime = 0; /* if nonzero, minimum of active blocktime values */
crypto_uint32 blockid[OUTGOING]; /* ID of last message sending this block */

#define INCOMING 64 /* must be power of 2 */
long long messagenum = 0; /* number of messages in incoming queue */
long long messagefirst = 0; /* position of first message; circular queue */
unsigned char messagelen[INCOMING]; /* times 16 */
unsigned char message[INCOMING][1088];
unsigned char messagetodo[2048];
long long messagetodolen = 0;

long long receivebytes = 0; /* number of initial bytes fully received */
long long receivewritten = 0; /* within receivebytes, number of bytes given to child */
crypto_uint16 receiveeof = 0; /* 0, 2048, 4096 */
long long receivetotalbytes = 0; /* total number of bytes in stream, if receiveeof */
unsigned char receivebuf[131072]; /* circular queue beyond receivewritten; size must be power of 2 */
unsigned char receivevalid[131072]; /* 1 for byte successfully received; XXX: use buddy structure to speed this up */

long long maxblocklen = 512;
crypto_uint32 nextmessageid = 1;

unsigned char buf[4096];

long long lastblocktime = 0;
long long nsecperblock = 1000000000;
long long lastspeedadjustment = 0;
long long lastedge = 0;
long long lastdoubling = 0;

long long rtt;
long long rtt_delta;
long long rtt_average = 0;
long long rtt_deviation = 0;
long long rtt_lowwater = 0;
long long rtt_highwater = 0;
long long rtt_timeout = 1000000000;
long long rtt_seenrecenthigh = 0;
long long rtt_seenrecentlow = 0;
long long rtt_seenolderhigh = 0;
long long rtt_seenolderlow = 0;
long long rtt_phase = 0;

long long lastpanic = 0;

void earliestblocktime_compute(void) /* XXX: use priority queue */
{
  long long i;
  long long pos;
  earliestblocktime = 0;
  for (i = 0;i < blocknum;++i) {
    pos = (blockfirst + i) & (OUTGOING - 1);
    if (blocktime[pos]) {
      if (!earliestblocktime)
        earliestblocktime = blocktime[pos];
      else
        if (blocktime[pos] < earliestblocktime)
	  earliestblocktime = blocktime[pos];
    }
  }
}

void acknowledged(unsigned long long start,unsigned long long stop)
{
  long long i;
  long long pos;
  if (stop == start) return;
  for (i = 0;i < blocknum;++i) {
    pos = (blockfirst + i) & (OUTGOING - 1);
    if (blockpos[pos] >= start && blockpos[pos] + blocklen[pos] <= stop) {
      blocktime[pos] = 0;
      totalblocktransmissions += blocktransmissions[pos];
      totalblocks += 1;
    }
  }
  while (blocknum) {
    pos = blockfirst & (OUTGOING - 1);
    if (blocktime[pos]) break;
    sendacked += blocklen[pos];
    sendbytes -= blocklen[pos];
    sendprocessed -= blocklen[pos];
    ++blockfirst;
    --blocknum;
  }
  if (sendeof)
    if (start == 0)
      if (stop > sendacked + sendbytes)
	if (!sendeofacked) {
          sendeofacked = 1;
	}
  earliestblocktime_compute();
}

int main(int argc,char **argv)
{
  long long pos;
  long long len;
  long long u;
  long long r;
  long long i;
  long long k;
  long long recent;
  long long nextaction;
  long long timeout;
  struct pollfd *q;
  struct pollfd *watch8;
  struct pollfd *watchtochild;
  struct pollfd *watchfromchild;

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
      if (*x == 'c') { flagserver = 0; wantping = 2; continue; }
      if (*x == 'C') { flagserver = 0; wantping = 1; continue; }
      if (*x == 's') { flagserver = 1; wantping = 0; continue; }
      die_usage(0);
    }
  }
  if (!*++argv) die_usage("missing prog");

  for (;;) {
    r = open_read("/dev/null");
    if (r == -1) die_fatal("unable to open /dev/null",0,0);
    if (r > 9) { close(r); break; }
  }

  if (open_pipe(tochild) == -1) die_fatal("unable to create pipe",0,0);
  if (open_pipe(fromchild) == -1) die_fatal("unable to create pipe",0,0);

  blocking_enable(tochild[0]);
  blocking_enable(fromchild[1]);

  child = fork();
  if (child == -1) die_fatal("unable to fork",0,0);
  if (child == 0) {
    close(8);
    close(9);
    if (flagserver) {
      close(0);
      if (dup(tochild[0]) != 0) die_fatal("unable to dup",0,0);
      close(1);
      if (dup(fromchild[1]) != 1) die_fatal("unable to dup",0,0);
    } else {
      close(6);
      if (dup(tochild[0]) != 6) die_fatal("unable to dup",0,0);
      close(7);
      if (dup(fromchild[1]) != 7) die_fatal("unable to dup",0,0);
    }
    signal(SIGPIPE,SIG_DFL);
    execvp(*argv,argv);
    die_fatal("unable to run",*argv,0);
  }

  close(tochild[0]);
  close(fromchild[1]);

  recent = nanoseconds();
  lastspeedadjustment = recent;
  if (flagserver) maxblocklen = 1024;

  for (;;) {
    if (sendeofacked)
      if (receivewritten == receivetotalbytes)
        if (receiveeof)
          if (tochild[1] < 0)
	    break; /* XXX: to re-ack should enter a TIME-WAIT state here */

    q = p;

    watch8 = q;
    if (watch8) { q->fd = 8; q->events = POLLIN; ++q; }

    watchtochild = q;
    if (tochild[1] < 0) watchtochild = 0;
    if (receivewritten >= receivebytes) watchtochild = 0;
    if (watchtochild) { q->fd = tochild[1]; q->events = POLLOUT; ++q; }

    watchfromchild = q;
    if (sendeof) watchfromchild = 0;
    if (sendbytes + 4096 > sizeof sendbuf) watchfromchild = 0;
    if (watchfromchild) { q->fd = fromchild[0]; q->events = POLLIN; ++q; }

    nextaction = recent + 60000000000LL;
    if (wantping == 1) nextaction = recent + 1000000000;
    if (wantping == 2)
      if (nextaction > lastblocktime + nsecperblock) nextaction = lastblocktime + nsecperblock;
    if (blocknum < OUTGOING)
      if (!(sendeof ? sendeofprocessed : sendprocessed >= sendbytes))
        if (nextaction > lastblocktime + nsecperblock) nextaction = lastblocktime + nsecperblock;
    if (earliestblocktime)
      if (earliestblocktime + rtt_timeout > lastblocktime + nsecperblock)
        if (earliestblocktime + rtt_timeout < nextaction)
	  nextaction = earliestblocktime + rtt_timeout;

    if (messagenum)
      if (!watchtochild)
        nextaction = 0;

    if (nextaction <= recent)
      timeout = 0;
    else
      timeout = (nextaction - recent) / 1000000 + 1;

    if (poll(p,q - p,timeout) < 0) {
      watch8 = 0;
      watchtochild = 0;
      watchfromchild = 0;
    } else {
      if (watch8) if (!watch8->revents) watch8 = 0;
      if (watchtochild) if (!watchtochild->revents) watchtochild = 0;
      if (watchfromchild) if (!watchfromchild->revents) watchfromchild = 0;
    }

    /* XXX: keepalives */

    do { /* try receiving data from child: */
      if (!watchfromchild) break;
      if (sendeof) break;
      if (sendbytes + 4096 > sizeof sendbuf) break;

      pos = (sendacked & (sizeof sendbuf - 1)) + sendbytes;
      if (pos < sizeof sendbuf) {
        r = read(fromchild[0],sendbuf + pos,sizeof sendbuf - pos);
      } else {
        r = read(fromchild[0],sendbuf + pos - sizeof sendbuf,sizeof sendbuf - sendbytes);
      }
      if (r == -1) if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) break;
      if (r < 0) { sendeof = 4096; break; }
      if (r == 0) { sendeof = 2048; break; }
      sendbytes += r;
      if (sendbytes >= 1152921504606846976LL) die_internalerror();
    } while(0);

    recent = nanoseconds();

    do { /* try re-sending an old block: */
      if (recent < lastblocktime + nsecperblock) break;
      if (earliestblocktime == 0) break;
      if (recent < earliestblocktime + rtt_timeout) break;

      for (i = 0;i < blocknum;++i) {
	pos = (blockfirst + i) & (OUTGOING - 1);
        if (blocktime[pos] == earliestblocktime) {
	  if (recent > lastpanic + 4 * rtt_timeout) {
	    nsecperblock *= 2;
	    lastpanic = recent;
	    lastedge = recent;
	  }
	  goto sendblock;
        }
      }
    } while(0);

    do { /* try sending a new block: */
      if (recent < lastblocktime + nsecperblock) break;
      if (blocknum >= OUTGOING) break;
      if (!wantping)
        if (sendeof ? sendeofprocessed : sendprocessed >= sendbytes) break;
      /* XXX: if any Nagle-type processing is desired, do it here */

      pos = (blockfirst + blocknum) & (OUTGOING - 1);
      ++blocknum;
      blockpos[pos] = sendacked + sendprocessed;
      blocklen[pos] = sendbytes - sendprocessed;
      if (blocklen[pos] > maxblocklen) blocklen[pos] = maxblocklen;
      if ((blockpos[pos] & (sizeof sendbuf - 1)) + blocklen[pos] > sizeof sendbuf)
        blocklen[pos] = sizeof sendbuf - (blockpos[pos] & (sizeof sendbuf - 1));
	/* XXX: or could have the full block in post-buffer space */
      sendprocessed += blocklen[pos];
      blockeof[pos] = 0;
      if (sendprocessed == sendbytes) {
        blockeof[pos] = sendeof;
	if (sendeof) sendeofprocessed = 1;
      }
      blocktransmissions[pos] = 0;

      sendblock:

      blocktransmissions[pos] += 1;
      blocktime[pos] = recent;
      blockid[pos] = nextmessageid;
      if (!++nextmessageid) ++nextmessageid;

      /* constraints: u multiple of 16; u >= 16; u <= 1088; u >= 48 + blocklen[pos] */
      u = 64 + blocklen[pos];
      if (u <= 192) u = 192;
      else if (u <= 320) u = 320;
      else if (u <= 576) u = 576;
      else if (u <= 1088) u = 1088;
      else die_internalerror();
      if (blocklen[pos] < 0 || blocklen[pos] > 1024) die_internalerror();

      byte_zero(buf + 8,u);
      buf[7] = u / 16;
      uint32_pack(buf + 8,blockid[pos]);
      /* XXX: include any acknowledgments that have piled up */
      uint16_pack(buf + 46,blockeof[pos] | (crypto_uint16) blocklen[pos]);
      uint64_pack(buf + 48,blockpos[pos]);
      byte_copy(buf + 8 + u - blocklen[pos],blocklen[pos],sendbuf + (blockpos[pos] & (sizeof sendbuf - 1)));

      if (writeall(9,buf + 7,u + 1) == -1) die_fatal("unable to write descriptor 9",0,0);
      lastblocktime = recent;
      wantping = 0;

      earliestblocktime_compute();
    } while(0);

    do { /* try receiving messages: */
      if (!watch8) break;
      r = read(8,buf,sizeof buf);
      if (r == -1) if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) break;
      if (r == 0) die_badmessage();
      if (r < 0) die_fatal("unable to read from file descriptor 8",0,0);
      for (k = 0;k < r;++k) {
        messagetodo[messagetodolen++] = buf[k];
	u = 16 * (unsigned long long) messagetodo[0];
	if (u < 16) die_badmessage();
	if (u > 1088) die_badmessage();
	if (messagetodolen == 1 + u) {
	  if (messagenum < INCOMING) {
	    pos = (messagefirst + messagenum) & (INCOMING - 1);
	    messagelen[pos] = messagetodo[0];
	    byte_copy(message[pos],u,messagetodo + 1);
	    ++messagenum;
	  } else {
	    ; /* drop tail */
	  }
	  messagetodolen = 0;
	}
      }
    } while(0);

    do { /* try processing a message: */
      if (!messagenum) break;
      if (tochild[1] >= 0 && receivewritten < receivebytes) break;

      maxblocklen = 1024;

      pos = messagefirst & (INCOMING - 1);
      len = 16 * (unsigned long long) messagelen[pos];
      do { /* handle this message if it's comprehensible: */
	unsigned long long D;
	unsigned long long SF;
	unsigned long long startbyte;
	unsigned long long stopbyte;
	crypto_uint32 id;
	long long i;

        if (len < 48) break;
        if (len > 1088) break;

	id = uint32_unpack(message[pos] + 4);
	for (i = 0;i < blocknum;++i) {
	  k = (blockfirst + i) & (OUTGOING - 1);
	  if (blockid[k] == id) {
	    rtt = recent - blocktime[k];
	    if (!rtt_average) {
	      nsecperblock = rtt;
	      rtt_average = rtt;
	      rtt_deviation = rtt / 2;
	      rtt_highwater = rtt;
	      rtt_lowwater = rtt;
	    }

	    /* Jacobson's retransmission timeout calculation: */
	    rtt_delta = rtt - rtt_average;
	    rtt_average += rtt_delta / 8;
	    if (rtt_delta < 0) rtt_delta = -rtt_delta;
	    rtt_delta -= rtt_deviation;
	    rtt_deviation += rtt_delta / 4;
	    rtt_timeout = rtt_average + 4 * rtt_deviation;
	    /* adjust for delayed acks with anti-spiking: */
	    rtt_timeout += 8 * nsecperblock;

	    /* recognizing top and bottom of congestion cycle: */
	    rtt_delta = rtt - rtt_highwater;
	    rtt_highwater += rtt_delta / 1024;
	    rtt_delta = rtt - rtt_lowwater;
	    if (rtt_delta > 0) rtt_lowwater += rtt_delta / 8192;
	    else rtt_lowwater += rtt_delta / 256;

	    if (rtt_average > rtt_highwater + 5000000) rtt_seenrecenthigh = 1;
	    else if (rtt_average < rtt_lowwater) rtt_seenrecentlow = 1;

	    if (recent >= lastspeedadjustment + 16 * nsecperblock) {
	      if (recent - lastspeedadjustment > 10000000000LL) {
	        nsecperblock = 1000000000; /* slow restart */
		nsecperblock += randommod(nsecperblock / 8);
	      }

	      lastspeedadjustment = recent;

	      if (nsecperblock >= 131072) {
	        /* additive increase: adjust 1/N by a constant c */
	        /* rtt-fair additive increase: adjust 1/N by a constant c every nanosecond */
	        /* approximation: adjust 1/N by cN every N nanoseconds */
	        /* i.e., N <- 1/(1/N + cN) = N/(1 + cN^2) every N nanoseconds */
	        if (nsecperblock < 16777216) {
		  /* N/(1+cN^2) approx N - cN^3 */
		  u = nsecperblock / 131072;
	          nsecperblock -= u * u * u;
	        } else {
	          double d = nsecperblock;
	          nsecperblock = d/(1 + d*d / 2251799813685248.0);
	        }
	      }

	      if (rtt_phase == 0) {
	        if (rtt_seenolderhigh) {
		  rtt_phase = 1;
		  lastedge = recent;
	          nsecperblock += randommod(nsecperblock / 4);
		}
	      } else {
	        if (rtt_seenolderlow) {
		  rtt_phase = 0;
	        }
	      }

	      rtt_seenolderhigh = rtt_seenrecenthigh;
	      rtt_seenolderlow = rtt_seenrecentlow;
	      rtt_seenrecenthigh = 0;
	      rtt_seenrecentlow = 0;
	    }

	    do {
	      if (recent - lastedge < 60000000000LL) {
	        if (recent < lastdoubling + 4 * nsecperblock + 64 * rtt_timeout + 5000000000LL) break;
	      } else {
	        if (recent < lastdoubling + 4 * nsecperblock + 2 * rtt_timeout) break;
	      }
	      if (nsecperblock <= 65535) break;

              nsecperblock /= 2;
	      lastdoubling = recent;
	      if (lastedge) lastedge = recent;
	    } while(0);
	  }
	}

	stopbyte = uint64_unpack(message[pos] + 8);
	acknowledged(0,stopbyte);
	startbyte = stopbyte + (unsigned long long) uint32_unpack(message[pos] + 16);
	stopbyte = startbyte + (unsigned long long) uint16_unpack(message[pos] + 20);
	acknowledged(startbyte,stopbyte);
	startbyte = stopbyte + (unsigned long long) uint16_unpack(message[pos] + 22);
	stopbyte = startbyte + (unsigned long long) uint16_unpack(message[pos] + 24);
	acknowledged(startbyte,stopbyte);
	startbyte = stopbyte + (unsigned long long) uint16_unpack(message[pos] + 26);
	stopbyte = startbyte + (unsigned long long) uint16_unpack(message[pos] + 28);
	acknowledged(startbyte,stopbyte);
	startbyte = stopbyte + (unsigned long long) uint16_unpack(message[pos] + 30);
	stopbyte = startbyte + (unsigned long long) uint16_unpack(message[pos] + 32);
	acknowledged(startbyte,stopbyte);
	startbyte = stopbyte + (unsigned long long) uint16_unpack(message[pos] + 34);
	stopbyte = startbyte + (unsigned long long) uint16_unpack(message[pos] + 36);
	acknowledged(startbyte,stopbyte);

	D = uint16_unpack(message[pos] + 38);
	SF = D & (2048 + 4096);
	D -= SF;
	if (D > 1024) break;
	if (48 + D > len) break;

	startbyte = uint64_unpack(message[pos] + 40);
	stopbyte = startbyte + D;

	if (stopbyte > receivewritten + sizeof receivebuf) {
	  break;
	  /* of course, flow control would avoid this case */
	}

	if (SF) {
	  receiveeof = SF;
	  receivetotalbytes = stopbyte;
	}

	for (k = 0;k < D;++k) {
	  unsigned char ch = message[pos][len - D + k];
	  unsigned long long where = startbyte + k;
	  if (where >= receivewritten && where < receivewritten + sizeof receivebuf) {
	    receivevalid[where & (sizeof receivebuf - 1)] = 1;
	    receivebuf[where & (sizeof receivebuf - 1)] = ch;
	  }
	}
	for (;;) {
	  if (receivebytes >= receivewritten + sizeof receivebuf) break;
	  if (!receivevalid[receivebytes & (sizeof receivebuf - 1)]) break;
	  ++receivebytes;
	}

	if (!uint32_unpack(message[pos])) break; /* never acknowledge a pure acknowledgment */

	/* XXX: delay acknowledgments */
	u = 192;
        byte_zero(buf + 8,u);
        buf[7] = u / 16;
	byte_copy(buf + 12,4,message[pos]);
	if (receiveeof && receivebytes == receivetotalbytes) {
	  uint64_pack(buf + 16,receivebytes + 1);
	} else
	  uint64_pack(buf + 16,receivebytes);
	/* XXX: incorporate selective acknowledgments */
  
        if (writeall(9,buf + 7,u + 1) == -1) die_fatal("unable to write descriptor 9",0,0);
      } while(0);

      ++messagefirst;
      --messagenum;
    } while(0);

    do { /* try sending data to child: */
      if (!watchtochild) break;
      if (tochild[1] < 0) { receivewritten = receivebytes; break; }
      if (receivewritten >= receivebytes) break;

      pos = receivewritten & (sizeof receivebuf - 1);
      len = receivebytes - receivewritten;
      if (pos + len > sizeof receivebuf) len = sizeof receivebuf - pos;
      r = write(tochild[1],receivebuf + pos,len);
      if (r == -1) if (errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN) break;
      if (r <= 0) {
        close(tochild[1]);
        tochild[1] = -1;
	break;
      }
      byte_zero(receivevalid + pos,r);
      receivewritten += r;
    } while(0);

    do { /* try closing pipe to child: */
      if (!receiveeof) break;
      if (receivewritten < receivetotalbytes) break;
      if (tochild[1] < 0) break;

      if (receiveeof == 4096)
        ; /* XXX: UNIX doesn't provide a way to signal an error through a pipe */
      close(tochild[1]);
      tochild[1] = -1;
    } while(0);

  }


  do {
    r = waitpid(child,&childstatus,0);
  } while (r == -1 && errno == EINTR);

  if (!WIFEXITED(childstatus)) { errno = 0; die_fatal("process killed by signal",0,0); }
  return WEXITSTATUS(childstatus);
}
