#ifndef OPEN_H
#define OPEN_H

extern int open_read(const char *);
extern int open_write(const char *);
extern int open_lock(const char *);
extern int open_cwd(void);
extern int open_pipe(int *);

#endif
