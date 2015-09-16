/**
 * @file   main.c
 * @author Benjamin James
 * @date   15 September 2015
 *
 * @brief  Main entry point for compiler
 */

#include <stdlib.h>
#include <string.h>
#include "common.h"

/**
 * @brief main
 *
 * @param argc the number of arguments
 * @param argv the argument strings
 * @return the success of the program (0 on success)
 */
int main(int argc, char **argv)
{
	int stack_size = 1000;
	int array_size = 1000;
	int mode = COMPILE | ASSEMBLE | LINK;
	char *output_file = NULL;
	char *input_file = NULL;
	FILE *in = NULL;
	FILE *out = NULL;
	int ret = my_getopt(argc, argv, &stack_size, &array_size, &output_file, &input_file, &mode);
	if (ret < 0)
		return -1;
	if (get_in(mode, input_file, &in) < 0)
		return -1;
	if (get_as_file(mode, &out, output_file, input_file) < 0)
		return -1;
	compile(in, out, stack_size, array_size);
	if (in != stdin && in != NULL)
		fclose(in);
	close_as_file(mode, out);

	if (!(mode & ASSEMBLE))
		goto end;
	char *as_file = NULL;
	if (get_as(mode, output_file, input_file, &as_file) == NULL && !(mode & PIPE_OUT))
		return -1;
	char *obj_file = NULL;
	if (get_obj(mode, output_file, input_file, &obj_file) == NULL)
		return -1;
	if (!(mode & PIPE_OUT))
		assemble(mode, as_file, obj_file);
	if (!(mode & LINK)) {
		if (as_file)
			free(as_file);
		if (obj_file)
			free(obj_file);
		goto end;
	}
	if (as_file) {
		remove(as_file);
		free(as_file);
	}
	char buf[256];
	if (output_file == NULL)
		snprintf(buf, 256, "ld %s", obj_file);
	else
		snprintf(buf, 256, "ld %s -o %s", obj_file, output_file);
	ret = system(buf);
	if (ret == 0)
		ret = remove(obj_file);
	free(obj_file);
end:
	return ret;
}
