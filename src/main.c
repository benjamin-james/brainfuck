#include <stdlib.h>
#include <string.h>
#include "common.h"

int main(int argc, char **argv)
{
	int stack_size = 1000;
	int array_size = 1000;
	int mode = COMPILE | ASSEMBLE | LINK;
	char *output_file = NULL;
	char *input_file = NULL;
	FILE *in = stdin;
	FILE *out = stdout;
	int ret = my_getopt(argc, argv, &stack_size, &array_size, &output_file, &input_file, &mode);
	if (ret < 0)
		return -1;
	if (input_file == NULL && !(mode & PIPE_IN)) {
		fprintf(stderr, "%s: fatal error: no input files\ncompilation terminated.\n", *argv);
		return -1;
	}
	if (!(mode & PIPE_IN)) {
		in = fopen(input_file, "r");
		if (mode & VERBOSE)
			printf("opening file %s\n", input_file);
	}
	if (in == NULL) {
		fprintf(stderr, "error: couldn't open file %s: ", input_file);
		perror(NULL);
		return -1;
	}
	if (!(mode & PIPE_OUT)) {
		if (output_file == NULL) {
			output_file = malloc(100);
			memcpy(output_file, "output", 7);
		}
		out = fopen(output_file, "w");
	}
	if (out == NULL) {
		fprintf(stderr, "error: couldn't open file %s: ", output_file);
		perror(NULL);
		return -1;
	}
	compile(in, out, stack_size, array_size);
	fclose(in);
	fclose(out);
	return 0;
}
