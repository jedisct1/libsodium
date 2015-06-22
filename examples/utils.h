/*
 * Utility functions shared by all the demo programs.
 */
#ifndef UTILS_H
#define UTILS_H

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sodium.h>

#define MAX_INPUT_LEN 4096

/*
 * print_hex() is a wrapper around sodium_bin2hex() which allocates
 * temporary memory then immediately prints the result followed by \n
 */
static void
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
    puts(hex);
    free(hex);
}

/*
 * Display a prompt for input by user. It will save the input into a buffer
 * of a specific size with room for the null terminator while removing
 * trailing newline characters.
 */
static size_t
prompt_input(const char *prompt, char *input, const size_t max_input_len,
             int variable_length)
{
    char   input_tmp[MAX_INPUT_LEN + 1U];
    size_t actual_input_len;

    if (variable_length != 0) {
        printf("\nEnter %s (%zu bytes max) > ", prompt, max_input_len);
    } else {
        printf("\nEnter %s (%zu bytes) > ", prompt, max_input_len);
    }
    fflush(stdout);
    fgets(input_tmp, sizeof input_tmp, stdin);
    actual_input_len = strlen(input_tmp);

    /* trim \n */
    if (actual_input_len > 0 && input_tmp[actual_input_len - 1] == '\n') {
        input_tmp[actual_input_len - 1] = '\0';
        --actual_input_len;
    }

    if (actual_input_len > max_input_len) {
        printf("Warning: truncating input to %zu bytes\n\n", max_input_len);
        actual_input_len = max_input_len;
    } else if (actual_input_len < max_input_len && variable_length == 0) {
        printf("Warning: %zu bytes expected, %zu bytes given: padding with zeros\n\n",
               max_input_len, actual_input_len);
        memset(input, 0, max_input_len);
    } else {
        printf("Length: %zu bytes\n\n", actual_input_len);
    }

    memcpy(input, input_tmp, actual_input_len);
    if (variable_length == 0) {
        return max_input_len;
    } else {
        return actual_input_len;
    }
}

/*
 * Display whether the function was sucessful or failed.
 */
static void
print_verification(int ret)
{
    if (ret == 0)
        puts("Success!\n");
    else
        puts("Failure.\n");
}

static void
init(void)
{
    sodium_init();
    printf("Using libsodium %s\n", sodium_version_string());
}

#endif /* UTILS_H */
