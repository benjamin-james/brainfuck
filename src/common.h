#ifndef COMMON_H
#define COMMON_H

#include <stdio.h> /* FILE */

#define COMPILE (1 << 0)
#define ASSEMBLE (1 << 1)
#define LINK (1 << 2)
#define PIPE_OUT (1 << 3)
#define PIPE_IN (1 << 4)
#define VERBOSE (1 << 5)

int is_uint(const char *str);
void print_help(void);
int my_getopt(int argc, char **argv, int *stack_size, int *array_size, char **output_file, char **input_file, int *mode);

int compile(FILE *in, FILE *out, int stack_size, int array_size);

#endif
