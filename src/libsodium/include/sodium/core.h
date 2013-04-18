
#ifndef __SODIUM_CORE_H__
#define __SODIUM_CORE_H__

typedef struct sodium_options sodium_options;

int  sodium_init(const sodium_options *options);
int  sodium_reinit(void);
void sodium_shutdown(void);

#endif
