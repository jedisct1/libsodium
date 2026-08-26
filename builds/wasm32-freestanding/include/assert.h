#undef assert

#ifdef NDEBUG
#define assert(cond) ((void)0)
#else
#define assert(cond) ((cond) ? (void)0 : __builtin_trap())
#endif
