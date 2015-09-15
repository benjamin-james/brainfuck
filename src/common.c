#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

int is_uint(const char *str)
{
	if (!*str)
		return 0;
	for (;*str; str++)
		if (!isdigit(*str))
			return 0;
	return 1;
}
void print_help(void)
{
	fprintf(stderr, "You need help!\n");
}
int my_getopt(int argc, char **argv, int *stack_size, int *array_size, char **output_file, char **input_file, int *mode)
{
	int i;
	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "-s")) {
			if (i + 1 < argc && is_uint(argv[i + 1])) {
				*stack_size = atoi(argv[++i]);
			} else {
				fprintf(stderr, "%s: option requires a numerical argument -- 's'\n", *argv);
				return -1;
			}
		} else if (!strcmp(argv[i], "-a")) {
			if (i + 1 < argc && is_uint(argv[i + 1])) {
				*array_size = atoi(argv[++i]);
			} else {
				fprintf(stderr, "%s: option requires a numerical argument -- 'a'\n", *argv);
				return -1;
			}
		} else if (!strcmp(argv[i], "-c")) {
			*mode &= ~(LINK);
		} else if (!strcmp(argv[i], "-S")) {
			*mode &= ~(LINK | ASSEMBLE);
		} else if (!strcmp(argv[i], "-o")) {
			if (i + 1 < argc) {
				*output_file = argv[++i];
				*mode &= ~(PIPE_OUT);
			} else {
				fprintf(stderr, "%s: option requires an argument -- 'o'\n", *argv);
				return -1;
			}
		} else if (!strcmp(argv[i], "-v")) {
			*mode |= VERBOSE;
		} else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
			print_help();
			return 0;
		} else if (!strcmp(argv[i], "-pipe")) {
			*mode |= PIPE_OUT;
		} else if (!strcmp(argv[i], "-i")) {
			*mode |= PIPE_IN;
		} else {
			*input_file = argv[i];
			*mode &= ~(PIPE_IN);
		}
	}
	return *mode;
}
