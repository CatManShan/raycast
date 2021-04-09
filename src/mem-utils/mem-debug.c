#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#undef MEM_DEBUG // MEM_DEBUG shouldn't be defined here
#include "mem-debug.h"

static bool initialized = false;

static struct PointerStack {
	void **arr;
	size_t length;
	size_t size;
} pointer_list = {NULL, 0, 0};

static FILE *out_stream = NULL;

static void pointer_list_init();
static void pointer_list_destroy();
static void pointer_list_add(void *ptr);
static void pointer_list_remove(void *ptr);
static size_t pointer_list_find(void *ptr);

void debug_start()
{
	if (out_stream == NULL) {
		out_stream = stdout;
	}

	pointer_list_init();

	initialized = true;
}

void debug_end()
{
	if (out_stream != stdout) {
		out_stream = NULL;
	}

	pointer_list_destroy();

	initialized = false;
}

void *debug_malloc(size_t size, const char *file, size_t line)
{
	if (initialized) {
		void *ptr = malloc(size);

		pointer_list_add(ptr);

		fprintf(out_stream, "malloc: %p %s %zd\n", ptr, file, line);
		return ptr;
	} else {
		return malloc(size);
	}
}

void debug_free(void *ptr, const char *file, size_t line)
{
	if (initialized) {
	pointer_list_remove(ptr);

	fprintf(out_stream, "free: %p %s %zd\n", ptr, file, line);
	free(ptr);
	} else {
		free(ptr);
	}
}

void *debug_calloc(size_t nmemb, size_t size, const char *file, size_t line)
{
	if (initialized) {
	void *ptr = calloc(nmemb, size);

	pointer_list_add(ptr);

	fprintf(out_stream, "calloc: %p %s %zd\n", ptr, file, line);
	return ptr;
	} else {
		return calloc(nmemb, size);
	}
}

void *debug_realloc(void *ptr, size_t size, const char *file, size_t line)
{
	if (initialized) {
	void *ptr_new = realloc(ptr, size);

	if (ptr_new != ptr) {
		pointer_list_remove(ptr);
		pointer_list_add(ptr_new);
	}

	fprintf(out_stream, "realloc: %p %p %s %s %zd\n", ptr, ptr_new, (ptr_new == ptr) ? "same" : "diff", file, line);
	return ptr_new;
	} else {
		return realloc(ptr, size);
	}
}

void debug_print_allocated()
{
	if (initialized) {
		for (size_t index = 0; index < pointer_list.length; index++) {
			fprintf(out_stream, "%p\n", pointer_list.arr[index]);
		}
	} else if (out_stream != NULL) {
		fprintf(out_stream, "Mem debug not initialized");
	}
}

void debug_set_out_stream(FILE *stream)
{
	out_stream = stream;
}

void pointer_list_init()
{
	pointer_list.size = 8;
	pointer_list.arr = malloc(pointer_list.size * sizeof pointer_list.arr[0]);
}

void pointer_list_destroy()
{
	pointer_list.size = 0;
	pointer_list.size = 0;

	free(pointer_list.arr);
	pointer_list.arr = NULL;
}

void pointer_list_add(void *ptr)
{
	pointer_list.arr[pointer_list.length] = ptr;
	++pointer_list.length;

	if (pointer_list.length == pointer_list.size) {
		pointer_list.size *= 2;
		pointer_list.arr = realloc(pointer_list.arr, pointer_list.size * sizeof pointer_list.arr[0]);
	}
}

void pointer_list_remove(void *ptr)
{
	size_t index = pointer_list_find(ptr);
	
	if (index != (size_t) -1) {
		pointer_list.arr[index] = pointer_list.arr[pointer_list.length - 1];
		--pointer_list.length;

		if (pointer_list.length * 4 == pointer_list.size && pointer_list.size / 2 >= 8) {
			pointer_list.size /= 2;
			pointer_list.arr = realloc(pointer_list.arr, pointer_list.size * sizeof pointer_list.arr[0]);
		}
	}
}

size_t pointer_list_find(void *ptr)
{
	for (size_t index = 0; index < pointer_list.length; index++) {
		if (pointer_list.arr[index] == ptr) {
			return index;
		}
	}

	return -1;
}

