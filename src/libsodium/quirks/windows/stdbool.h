
#ifndef __STDBOOL_H__
#define __STDBOOL_H__

#if defined(__cplusplus) || !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L
typedef signed char _Bool;
# undef  bool
# define bool _Bool

# ifndef __bool_true_false_are_defined
#  undef false
#  define false 0
#  undef true
#  define true 1
# endif
#endif

#endif
