/**
 * @file   compiler.c
 * @author Benjamin James
 * @date   15 September 2015
 *
 * @brief File containing the important
 * compile function
 */

#include <stdlib.h>

#include "common.h"

/**
 * @brief The size of the stack buffer
 */
#define BUF_SIZE 256

/**
 * @brief compiler function
 *
 * @param in Where to get the source code from
 * @param out Where to print the assembly code
 * @param stack_size The size of the loop stack
 * @param array_size The size of the brainfuck internal array
 * @return the success of this function, 0 on success, -1 on failure
 */
int compile(FILE *in, FILE *out, int stack_size, int array_size)
{
	int c, ret = 0;
	unsigned long label_num = 0L;
	int stack_ptr = 0;
	unsigned long *stack = malloc(stack_size * sizeof *stack);
	if (stack == NULL) {
		fprintf(stderr, "error: couldn't allocate stack (%lu bytes)\n", stack_size * sizeof *stack);
		ret = -1;
		goto cleanup;
	}
	char buf[BUF_SIZE];
	snprintf(buf, BUF_SIZE, "\t.lcomm buffer %d\n", array_size);
	fputs("\t.section .bss\n", out);
	fputs(buf, out);
	fputs("\n", out);
	fputs("\t.section .text\n", out);
	fputs("\t.globl _start\n", out);
	fputs("_start:\n", out);
	fputs("\tmovl $buffer, %edi\n", out);
	fputs("\n", out);
	while ((c = fgetc(in)) != EOF) {
		switch (c) {
		case '>':
			fputs("\tinc %edi\n", out);
			fputs("\n", out);
			break;
		case '<':
			fputs("\tdec %edi\n", out);
			fputs("\n", out);
			break;
		case '+':
			fputs("\tincb (%edi)\n", out);
			fputs("\n", out);
			break;
		case '-':
			fputs("\tdecb (%edi)\n", out);
			fputs("\n", out);
			break;
		case '.':
			fputs("\tmovl $4, %eax\n", out);
			fputs("\tmovl $1, %ebx\n", out);
			fputs("\tmovl %edi, %ecx\n", out);
			fputs("\tmovl $1, %edx\n", out);
			fputs("\tint $0x80\n", out);
			fputs("\n", out);
			break;
		case ',':
			fputs("\tmovl $3, %eax\n", out);
			fputs("\tmovl $0, %ebx\n", out);
			fputs("\tmovl %edi, %ecx\n", out);
			fputs("\tmovl $1, %edx\n", out);
			fputs("\tint $0x80\n", out);
			fputs("\n", out);
			break;
		case '[':
			if (stack_ptr == stack_size) {
				stack_size *= 2;
				unsigned long *temp = realloc(stack, stack_size * sizeof *stack);
				if (temp == NULL) {
					fprintf(stderr, "error: couldn't reallocate stack (%lu bytes)\n", stack_size * sizeof *stack);
					ret = -1;
					goto cleanup;
				}
				stack = temp;
			}
			stack[stack_ptr++] = ++label_num;
			fputs("\tcmpb $0, (%edi)\n", out);
			snprintf(buf, BUF_SIZE, "\tjz .LE%lu\n", label_num);
			fputs(buf, out);
			snprintf(buf, BUF_SIZE, ".LB%lu:\n", label_num);
			fputs(buf, out);
			break;
		case ']':
			fputs("\tcmpb $0, (%edi)\n", out);
			snprintf(buf, BUF_SIZE, "\tjnz .LB%lu\n", stack[--stack_ptr]);
			fputs(buf, out);
			snprintf(buf, BUF_SIZE, ".LE%lu:\n", stack[stack_ptr]);
			fputs(buf, out);
			break;
		default:
			break;
		}
	}
	fputs("\n", out);
	fputs("\tmovl $1, %eax\n", out);
	fputs("\tmovl $0, %ebx\n", out);
	fputs("\tint $0x80\n", out);
	fputs("\n", out);
cleanup:
	if (stack)
		free(stack);
	return ret;
}
