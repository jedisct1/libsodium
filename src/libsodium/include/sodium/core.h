
#ifndef __SODIUM_CORE_H__
#define __SODIUM_CORE_H__

int  sodium_init(const struct sodium_options *options);
int  sodium_reinit(void);
void sodium_shutdown(void);

#endif
