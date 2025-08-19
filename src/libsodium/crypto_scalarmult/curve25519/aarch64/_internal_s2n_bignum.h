#ifdef __aarch64__

#ifdef __APPLE__
#   define S2N_BN_SYMBOL(NAME) _##NAME
#else
#   define S2N_BN_SYMBOL(name) name
#endif

#define S2N_BN_SYM_VISIBILITY_DIRECTIVE(name) .globl S2N_BN_SYMBOL(name)
#ifdef S2N_BN_HIDE_SYMBOLS
#   ifdef __APPLE__
#      define S2N_BN_SYM_PRIVACY_DIRECTIVE(name) .private_extern S2N_BN_SYMBOL(name)
#   else
#      define S2N_BN_SYM_PRIVACY_DIRECTIVE(name) .hidden S2N_BN_SYMBOL(name)
#   endif
#else
#   define S2N_BN_SYM_PRIVACY_DIRECTIVE(name)  /* NO-OP: S2N_BN_SYM_PRIVACY_DIRECTIVE */
#endif

#endif
