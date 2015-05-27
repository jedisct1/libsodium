/*
 * Utility functions shared by all the demo programs.
 */
#ifndef DEMO_UTILS_H
#define DEMO_UTILS_H

#include <stdlib.h>

#define BUFFER_SIZE 128 /* size of all input buffers in the demo */

void print_hex(const void *buf, const size_t len);
size_t prompt_input(char *prompt, char *buf, const size_t len);
void print_verification(int r);

#endif /* DEMO_UTILS_H */
