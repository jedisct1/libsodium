/*
 * These are the utility functions shared by all demo programs.
 */
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sodium.h>

#include "demo_utils.h"

/* ================================================================== *
 *                          utility functions                         *
 * ================================================================== */

/*
 * Print hex is a wrapper around sodium_bin2hex which allocates
 * temporary memory then immediately prints the result.
 */
void
print_hex(const void *bin, const size_t bin_len)
{
    char   *hex;
    size_t  hex_size;

    if (bin_len >= SIZE_MAX / 2) {
        abort();
    }
    hex_size = bin_len * 2 + 1;
    if ((hex = malloc(hex_size)) == NULL) {
        abort();
    }
    /* the library supplies a few utility functions like the one below */
    if (sodium_bin2hex(hex, hex_size, bin, bin_len) == NULL) {
        abort();
    }
    fputs(hex, stdout);
    free(hex);
}

/*
 * Display a prompt for input by user. It will save the input into a buffer
 * of a specific size with room for the null terminator while removing
 * trailing newline characters.
 */
size_t
prompt_input(char *prompt, char *input, const size_t max_input_len)
{
    size_t actual_input_len;

    fputs(prompt, stdout);
    fflush(stdout);
    fgets(input, max_input_len, stdin); /* grab input with room for \0 */

    actual_input_len = strlen(input);

    /* trim excess new line */
    if (actual_input_len > 0 && input[actual_input_len - 1] == '\n') {
        input[actual_input_len - 1] = '\0';
        --actual_input_len;
    }
    return actual_input_len;
}

/*
 * Print a message if the function was sucessful or failed.
 */
void
print_verification(int r)
{
    if (r == 0)
        puts("Success\n");
    else
        puts("Failure\n");
}
