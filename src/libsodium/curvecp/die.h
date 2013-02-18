#ifndef DIE_H
#define DIE_H

extern void die_9(int,const char *,const char *,const char *,const char *,const char *,const char *,const char *,const char *,const char *);

#define die_8(x,a,b,c,d,e,f,g,h) die_9(x,a,b,c,d,e,f,g,h,0)
#define die_7(x,a,b,c,d,e,f,g) die_8(x,a,b,c,d,e,f,g,0)
#define die_6(x,a,b,c,d,e,f) die_7(x,a,b,c,d,e,f,0)
#define die_5(x,a,b,c,d,e) die_6(x,a,b,c,d,e,0)
#define die_4(x,a,b,c,d) die_5(x,a,b,c,d,0)
#define die_3(x,a,b,c) die_4(x,a,b,c,0)
#define die_2(x,a,b) die_3(x,a,b,0)
#define die_1(x,a) die_2(x,a,0)
#define die_0(x) die_1(x,0)

#endif
