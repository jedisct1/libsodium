#ifndef E_H
#define E_H

#include <errno.h>

extern const char *e_str(int);

#ifndef EPERM
#define EPERM (-5001)
#endif
#ifndef ENOENT
#define ENOENT (-5002)
#endif
#ifndef ESRCH
#define ESRCH (-5003)
#endif
#ifndef EINTR
#define EINTR (-5004)
#endif
#ifndef EIO
#define EIO (-5005)
#endif
#ifndef ENXIO
#define ENXIO (-5006)
#endif
#ifndef E2BIG
#define E2BIG (-5007)
#endif
#ifndef ENOEXEC
#define ENOEXEC (-5008)
#endif
#ifndef EBADF
#define EBADF (-5009)
#endif
#ifndef ECHILD
#define ECHILD (-5010)
#endif
#ifndef EAGAIN
#define EAGAIN (-5011)
#endif
#ifndef EWOULDBLOCK
#define EWOULDBLOCK (-7011)
#endif
#ifndef ENOMEM
#define ENOMEM (-5012)
#endif
#ifndef EACCES
#define EACCES (-5013)
#endif
#ifndef EFAULT
#define EFAULT (-5014)
#endif
#ifndef ENOTBLK
#define ENOTBLK (-5015)
#endif
#ifndef EBUSY
#define EBUSY (-5016)
#endif
#ifndef EEXIST
#define EEXIST (-5017)
#endif
#ifndef EXDEV
#define EXDEV (-5018)
#endif
#ifndef ENODEV
#define ENODEV (-5019)
#endif
#ifndef ENOTDIR
#define ENOTDIR (-5020)
#endif
#ifndef EISDIR
#define EISDIR (-5021)
#endif
#ifndef EINVAL
#define EINVAL (-5022)
#endif
#ifndef ENFILE
#define ENFILE (-5023)
#endif
#ifndef EMFILE
#define EMFILE (-5024)
#endif
#ifndef ENOTTY
#define ENOTTY (-5025)
#endif
#ifndef ETXTBSY
#define ETXTBSY (-5026)
#endif
#ifndef EFBIG
#define EFBIG (-5027)
#endif
#ifndef ENOSPC
#define ENOSPC (-5028)
#endif
#ifndef ESPIPE
#define ESPIPE (-5029)
#endif
#ifndef EROFS
#define EROFS (-5030)
#endif
#ifndef EMLINK
#define EMLINK (-5031)
#endif
#ifndef EPIPE
#define EPIPE (-5032)
#endif
#ifndef EDOM
#define EDOM (-5033)
#endif
#ifndef ERANGE
#define ERANGE (-5034)
#endif
#ifndef EDEADLK
#define EDEADLK (-5035)
#endif
#ifndef EDEADLOCK
#define EDEADLOCK (-7035)
#endif
#ifndef ENAMETOOLONG
#define ENAMETOOLONG (-5036)
#endif
#ifndef ENOLCK
#define ENOLCK (-5037)
#endif
#ifndef ENOSYS
#define ENOSYS (-5038)
#endif
#ifndef ENOTEMPTY
#define ENOTEMPTY (-5039)
#endif
#ifndef ELOOP
#define ELOOP (-5040)
#endif
#ifndef ENOMSG
#define ENOMSG (-5042)
#endif
#ifndef EIDRM
#define EIDRM (-5043)
#endif
#ifndef ECHRNG
#define ECHRNG (-5044)
#endif
#ifndef EL2NSYNC
#define EL2NSYNC (-5045)
#endif
#ifndef EL3HLT
#define EL3HLT (-5046)
#endif
#ifndef EL3RST
#define EL3RST (-5047)
#endif
#ifndef ELNRNG
#define ELNRNG (-5048)
#endif
#ifndef EUNATCH
#define EUNATCH (-5049)
#endif
#ifndef ENOCSI
#define ENOCSI (-5050)
#endif
#ifndef EL2HLT
#define EL2HLT (-5051)
#endif
#ifndef EBADE
#define EBADE (-5052)
#endif
#ifndef EBADR
#define EBADR (-5053)
#endif
#ifndef EXFULL
#define EXFULL (-5054)
#endif
#ifndef ENOANO
#define ENOANO (-5055)
#endif
#ifndef EBADRQC
#define EBADRQC (-5056)
#endif
#ifndef EBADSLT
#define EBADSLT (-5057)
#endif
#ifndef EBFONT
#define EBFONT (-5059)
#endif
#ifndef ENOSTR
#define ENOSTR (-5060)
#endif
#ifndef ENODATA
#define ENODATA (-5061)
#endif
#ifndef ETIME
#define ETIME (-5062)
#endif
#ifndef ENOSR
#define ENOSR (-5063)
#endif
#ifndef ENONET
#define ENONET (-5064)
#endif
#ifndef ENOPKG
#define ENOPKG (-5065)
#endif
#ifndef EREMOTE
#define EREMOTE (-5066)
#endif
#ifndef ENOLINK
#define ENOLINK (-5067)
#endif
#ifndef EADV
#define EADV (-5068)
#endif
#ifndef ESRMNT
#define ESRMNT (-5069)
#endif
#ifndef ECOMM
#define ECOMM (-5070)
#endif
#ifndef EPROTO
#define EPROTO (-5071)
#endif
#ifndef EMULTIHOP
#define EMULTIHOP (-5072)
#endif
#ifndef EDOTDOT
#define EDOTDOT (-5073)
#endif
#ifndef EBADMSG
#define EBADMSG (-5074)
#endif
#ifndef EOVERFLOW
#define EOVERFLOW (-5075)
#endif
#ifndef ENOTUNIQ
#define ENOTUNIQ (-5076)
#endif
#ifndef EBADFD
#define EBADFD (-5077)
#endif
#ifndef EREMCHG
#define EREMCHG (-5078)
#endif
#ifndef ELIBACC
#define ELIBACC (-5079)
#endif
#ifndef ELIBBAD
#define ELIBBAD (-5080)
#endif
#ifndef ELIBSCN
#define ELIBSCN (-5081)
#endif
#ifndef ELIBMAX
#define ELIBMAX (-5082)
#endif
#ifndef ELIBEXEC
#define ELIBEXEC (-5083)
#endif
#ifndef EILSEQ
#define EILSEQ (-5084)
#endif
#ifndef ERESTART
#define ERESTART (-5085)
#endif
#ifndef ESTRPIPE
#define ESTRPIPE (-5086)
#endif
#ifndef EUSERS
#define EUSERS (-5087)
#endif
#ifndef ENOTSOCK
#define ENOTSOCK (-5088)
#endif
#ifndef EDESTADDRREQ
#define EDESTADDRREQ (-5089)
#endif
#ifndef EMSGSIZE
#define EMSGSIZE (-5090)
#endif
#ifndef EPROTOTYPE
#define EPROTOTYPE (-5091)
#endif
#ifndef ENOPROTOOPT
#define ENOPROTOOPT (-5092)
#endif
#ifndef EPROTONOSUPPORT
#define EPROTONOSUPPORT (-5093)
#endif
#ifndef ESOCKTNOSUPPORT
#define ESOCKTNOSUPPORT (-5094)
#endif
#ifndef EOPNOTSUPP
#define EOPNOTSUPP (-5095)
#endif
#ifndef EPFNOSUPPORT
#define EPFNOSUPPORT (-5096)
#endif
#ifndef EAFNOSUPPORT
#define EAFNOSUPPORT (-5097)
#endif
#ifndef EADDRINUSE
#define EADDRINUSE (-5098)
#endif
#ifndef EADDRNOTAVAIL
#define EADDRNOTAVAIL (-5099)
#endif
#ifndef ENETDOWN
#define ENETDOWN (-5100)
#endif
#ifndef ENETUNREACH
#define ENETUNREACH (-5101)
#endif
#ifndef ENETRESET
#define ENETRESET (-5102)
#endif
#ifndef ECONNABORTED
#define ECONNABORTED (-5103)
#endif
#ifndef ECONNRESET
#define ECONNRESET (-5104)
#endif
#ifndef ENOBUFS
#define ENOBUFS (-5105)
#endif
#ifndef EISCONN
#define EISCONN (-5106)
#endif
#ifndef ENOTCONN
#define ENOTCONN (-5107)
#endif
#ifndef ESHUTDOWN
#define ESHUTDOWN (-5108)
#endif
#ifndef ETOOMANYREFS
#define ETOOMANYREFS (-5109)
#endif
#ifndef ETIMEDOUT
#define ETIMEDOUT (-5110)
#endif
#ifndef ECONNREFUSED
#define ECONNREFUSED (-5111)
#endif
#ifndef EHOSTDOWN
#define EHOSTDOWN (-5112)
#endif
#ifndef EHOSTUNREACH
#define EHOSTUNREACH (-5113)
#endif
#ifndef EALREADY
#define EALREADY (-5114)
#endif
#ifndef EINPROGRESS
#define EINPROGRESS (-5115)
#endif
#ifndef ESTALE
#define ESTALE (-5116)
#endif
#ifndef EUCLEAN
#define EUCLEAN (-5117)
#endif
#ifndef ENOTNAM
#define ENOTNAM (-5118)
#endif
#ifndef ENAVAIL
#define ENAVAIL (-5119)
#endif
#ifndef EISNAM
#define EISNAM (-5120)
#endif
#ifndef EREMOTEIO
#define EREMOTEIO (-5121)
#endif
#ifndef EDQUOT
#define EDQUOT (-5122)
#endif
#ifndef ENOMEDIUM
#define ENOMEDIUM (-5123)
#endif
#ifndef EMEDIUMTYPE
#define EMEDIUMTYPE (-5124)
#endif
#ifndef ECANCELED
#define ECANCELED (-5125)
#endif
#ifndef ENOKEY
#define ENOKEY (-5126)
#endif
#ifndef EKEYEXPIRED
#define EKEYEXPIRED (-5127)
#endif
#ifndef EKEYREVOKED
#define EKEYREVOKED (-5128)
#endif
#ifndef EKEYREJECTED
#define EKEYREJECTED (-5129)
#endif
#ifndef EOWNERDEAD
#define EOWNERDEAD (-5130)
#endif
#ifndef ENOTRECOVERABLE
#define ENOTRECOVERABLE (-5131)
#endif
#ifndef ERFKILL
#define ERFKILL (-5132)
#endif
#ifndef EPROCLIM
#define EPROCLIM (-6067)
#endif
#ifndef EBADRPC
#define EBADRPC (-6072)
#endif
#ifndef ERPCMISMATCH
#define ERPCMISMATCH (-6073)
#endif
#ifndef EPROGUNAVAIL
#define EPROGUNAVAIL (-6074)
#endif
#ifndef EPROGMISMATCH
#define EPROGMISMATCH (-6075)
#endif
#ifndef EPROCUNAVAIL
#define EPROCUNAVAIL (-6076)
#endif
#ifndef EFTYPE
#define EFTYPE (-6079)
#endif
#ifndef EAUTH
#define EAUTH (-6080)
#endif
#ifndef ENEEDAUTH
#define ENEEDAUTH (-6081)
#endif
#ifndef ENOATTR
#define ENOATTR (-6087)
#endif
#ifndef ENOTCAPABLE
#define ENOTCAPABLE (-6093)
#endif

#endif
