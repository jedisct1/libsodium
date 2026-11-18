#ifndef _ERRNO_H
#define _ERRNO_H

#ifdef __cplusplus
extern "C" {
#endif

extern int errno;

#ifdef __cplusplus
}
#endif

#define EPERM     1
#define ENOENT    2
#define EINTR     4
#define EIO       5
#define ENXIO     6
#define EAGAIN    11
#define ENOMEM    12
#define EEXIST    17
#define EINVAL    22
#define EFBIG     27
#define ERANGE    34
#define ENOSYS    38
#define EBADMSG   74
#define EOVERFLOW 75
#define ENOTSUP   95

#endif
