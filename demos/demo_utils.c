/*
 * These are the utility functions shared by all demo programs.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sodium.h"             /* library header */

#include "demo_utils.h"         /* demo utility header */



/* ================================================================== *
 *                          utility functions                         *
 * ================================================================== */

/*
 * Print hex.
 */
void
print_hex(const void *buf, const size_t len)
{
    const unsigned char *b;
    char *p;
    
    b = buf;
    p = malloc((len * 2 + 1) * (sizeof *b));
    
    /* the library supplies a few utility functions like the one below */
    sodium_bin2hex(p, len * 2 + 1, b, len);
    fputs(p, stdout);
    free(p);
}

/*
 * Display a prompt for input by user. It will save the input into a buffer
 * of a specific size with room for the null terminator while removing
 * trailing newline characters.
 */
size_t
prompt_input(char *prompt, char *buf, const size_t len)
{
    size_t n;

    fputs(prompt, stdout);
    fgets(buf, len, stdin);             /* grab input with room for NULL */
    
    n = strlen(buf);
    if (buf[n - 1] == '\n') {           /* trim excess new line */
        buf[n - 1] = '\0';
        --n;
    }
    return n;
}

/*
 * Print a message if the function was sucessful or failed.
 */
void
print_verification(int r)
{
    if (r == 0) puts("Success\n");
    else        puts("Failure\n");
}

