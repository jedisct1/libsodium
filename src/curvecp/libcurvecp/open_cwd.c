#include "open.h"

int open_cwd(void)
{
  return open_read(".");
}
