/**
 * @file   common.c
 * @author Benjamin James
 * @date   15 September 2015
 *
 * @brief Common and utility functions
 */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

/**
 * @brief Checks if the given string
 * can represent a positive integer
 *
 * @param str The string to check
 * @return if the string is a positive integer or not
 */
int is_uint(const char *str)
{
	if (!*str) {
		return 0;
	}
	for (;*str; str++) {
		if (!isdigit(*str)) {
			return 0;
		}
	}
	return 1;
}

/**
 * @brief Prints the help message
 * when "-h" or "--help" are command
 * line arguments
 */
void print_help(void)
{
	fputs("Usage: bfc [input file] [options]\n", stderr);
	fputs("\n", stderr);
	fputs("\t-s [NUMBER]     sets the stack size to this number\n", stderr);
	fputs("\t-a [ARRSIZE]    sets the array size in the program to this number\n", stderr);
	fputs("\t-c              only compile and assemble\n", stderr);
	fputs("\t-S              only compile\n", stderr);
	fputs("\t-o [FILE]       specify an output file\n", stderr);
	fputs("\t-v              enable verbose output\n", stderr);
	fputs("\t-h, --help      print this help message\n", stderr);
	fputs("\t-i              get input from stdin\n", stderr);
	fputs("\t-pipe           pipe to assembler if -S is not set, otherwise print to stdout\n", stderr);
	fputs("\n", stderr);
}

/**
 * @brief Custom getopt function
 *
 * Since I don't want to break compatibility
 * with all systems by including unistd.h, this
 * function exists. It sets variables based on
 * command line arguments and flags.
 *
 * @param argc The amount of command line args
 * @param argv The command line args
 * @param stack_size A pointer to set a variable which determines the loop stack size
 * @param array_size A pointer to set a variable which determines the internal array size for the compiled program
 * @param output_file A pointer to set the argument of the -o option
 * @param input_file A pointer to set the file to compile
 * @param mode A pointer to the overall state of the program determined by the flags
 * @return mode, the state of the program
 */
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
			return -1;
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

/**
 * @brief Removes the file extension from a string,
 * if there is one at all.
 *
 * @param str The string to check the file extension of
 * @return The malloc'd string containing the filename without the extension
 */
char *remove_extenstion(const char *str)
{
	int strl = strlen(str);
	const char *s = str + strl - 1;
	for (; *s; s--) {
		if (*s == '/')
			break;
		if (*s == '.') {
			char *newstr = malloc(1 + s - str);
			memcpy(newstr, str, s - str);
			newstr[s - str] = '\0';
			return newstr;
		}
	}
	return strdup(str);
}

/**
 * @brief Gets the name of the assembly code file
 *
 * @param mode The state of the program
 * @param name The name of the file set as output by the -o option
 * @param original The name of the original file that is being compiled
 * @ret A pointer to the malloc'd name of the assembly code file, or NULL
 * if the compiler is piping out the assembly code
 * @return the malloc'd name of the assembly code file, or NULL
 * if the compiler is piping out the assembly code
 */
char *get_as(int mode, const char *name, const char *original, char **ret)
{
	if (name != NULL && ~(mode & ASSEMBLE))
		*ret = strdup(name);
	else if (mode & PIPE_OUT)
		*ret = NULL;
	else if (original != NULL) {
		char *base = remove_extenstion(original);
		int strl = strlen(base);
		char *new_base = realloc(base, strl + 3);
		if (new_base == NULL) {
			fprintf(stderr, "error: couldn't realloc string: ");
			perror(NULL);
			return (*ret = NULL);
		}
		base = new_base;
		*ret = strncat(base, ".s", strl + 3);
	} else
		*ret = strdup("a.out.s");
	return *ret;
}

/**
 * @brief Gets the name of the assembled file
 *
 * @param mode The state of the program
 * @param name The name of the file set as output by the -o option
 * @param original The name of the original file that is being compiled
 * @param ret A pointer to the malloc'd name of the assembled file if no errors occur
 * @ret The malloc'd name of the assembled file if no errors occur
 */
char *get_obj(int mode, const char *name, const char *original, char **ret)
{
	if (name != NULL && ~(mode & LINK)) {
		*ret = strdup(name);
	} else if (original != NULL) {
		char *base = remove_extenstion(original);
		int strl = strlen(base);
		char *new_base = realloc(base, strl + 3);
		if (new_base == NULL) {
			puts("error: couldn't realloc string");
			return NULL;
		}
		base = new_base;
		*ret = strncat(base, ".o", strl + 3);
	} else {
		*ret = strdup("a.out.o");
	}
	return *ret;
}

/**
 * @brief Gets the file pointer to the assembly code file
 *
 * If -pipe is specified and if the assembly will be assembled,
 * the file pointer will be pointing to the piped process assembling
 * the code concurrently.
 *
 * @param mode The state of the program
 * @param out The pointer to the file pointer to return
 * @param name The name of the file set as output by the -o option
 * @param input_name The name of the original file that is being compiled
 * @return 0 on success, -1 on failure
 */
int get_as_file(int mode, FILE **out, const char *name, const char *input_name)
{
	if (name != NULL && ~(mode & ASSEMBLE)) {
		*out = fopen(name, "w");
		if (*out == NULL) {
			fprintf(stderr, "error: couldn't open file %s: ", name);
			perror(NULL);
			return -1;
		}
	} else if ((mode & PIPE_OUT) && (mode & ASSEMBLE)) {
		char buf[100];
		char *foo = NULL;
		if (get_obj(mode, name, input_name, &foo) == NULL)
			return -1;
		snprintf(buf, 100, "as -g -o %s", foo);
		free(foo);
		*out = popen(buf, "w");
		if (*out == NULL) {
			fprintf(stderr, "error: couldn't open process %s: ", buf);
			perror(NULL);
			return -1;
		}
	} else if (mode & PIPE_OUT) {
		*out = stdout;
	} else {
		char *foo = NULL;
		if (get_as(mode, name, input_name, &foo) == NULL)
			return -1;
		*out = fopen(foo, "w");
		if (*out == NULL) {
			fprintf(stderr, "error: couldn't open file %s: ", foo);
			perror(NULL);
			return -1;
		}
		free(foo);
	}
	return 0;
}

/**
 * @brief Closes the file pointer to the assembly source file
 *
 * Its magic is in the fact that if this was a process the compier
 * was writing to, it waits for the process to end using pclose(3),
 *  but otherwise using fclose(3) to close the file.
 *
 * @param mode The state of the program
 * @param f The file pointer to close
 * @return if the file pointer was successfully closed
 */
int close_as_file(int mode, FILE *f)
{
	int ret = 0;
	if ((mode & PIPE_OUT) && (mode & ASSEMBLE))
		ret = pclose(f);
	else if (f != stdout)
		ret = fclose(f);
	f = NULL;
	return ret;
}

/**
 * @brief Wrapper around assembling the assembly code
 *
 * This also removes the assembly code file if we aren't linking
 * the object code afterwards.
 *
 * @param mode The state of the program
 * @param as_file The name of the assembly source code file
 * @param obj_file The name of the object code file that will be generated
 * @return the success of the assembler process
 */
int assemble(int mode, const char *as_file, const char *obj_file)
{
	if (!(mode & ASSEMBLE) || mode & PIPE_OUT)
		return 0;
	char buf[256];
	snprintf(buf, 256, "as %s -g -o %s", as_file, obj_file);
	int ret = system(buf);
	if (ret == 0 && !(mode & LINK)) {
		ret = remove(as_file);
	}
	return ret;
}

/**
 * @brief Gets the source code file pointer
 * and deals with IO errors
 *
 * @param mode The state of the program
 * @param input_file The file specified by the command line args
 * @param in The pointer to the file pointer to set
 */
int get_in(int mode, const char *input_file, FILE **in)
{
	if (input_file == NULL && !(mode & PIPE_IN)) {
		fprintf(stderr, "bfc: error: no input files\ncompilation terminated.\n");
		return -1;
	}
	if (mode & PIPE_IN)
		*in = stdin;
	else {
		*in = fopen(input_file, "r");
		if (mode & VERBOSE)
			printf("opening file %s\n", input_file);
	}
	if (*in == NULL) {
		fprintf(stderr, "error: couldn't open file %s: ", input_file);
		perror(NULL);
		return -1;
	}
	return 0;
}
