#ifndef SOCKET_H
#define SOCKET_H

extern int socket_udp(void);
extern int socket_bind(int,const unsigned char *,const unsigned char *);
extern int socket_send(int,const unsigned char *,long long,const unsigned char *,const unsigned char *);
extern long long socket_recv(int,unsigned char *,long long,unsigned char *,unsigned char *);

#endif
