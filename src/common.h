/**
 * @file   common.h
 * @author Benjamin James
 * @date   15 September 2015
 *
 * @brief Header for extern functions
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdio.h> /* FILE */

/**
 * @brief Macros for determining state of the program
 */
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
char *remove_extenstion(const char *str);
char *get_as(int mode, const char *name, const char *original, char **ret);
char *get_obj(int mode, const char *name, const char *original, char **ret);
int get_as_file(int mode, FILE **out, const char *name, const char *input_name);
int close_as_file(int mode, FILE *f);
int assemble(int mode, const char *as_file, const char *obj_file);
int get_in(int mode, const char *input_file, FILE **in);
#endif
