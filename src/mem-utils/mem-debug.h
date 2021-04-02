#ifndef mem_debug_h
#define mem_debug_h

#include <stddef.h>
#include <stdio.h>

#ifdef MEM_DEBUG
#define malloc(size) debug_malloc(size, __FILE__, __LINE__)
#define free(ptr) debug_free(ptr, __FILE__, __LINE__)
#define calloc(nmemb, size) debug_calloc(nmemb, size, __FILE__, __LINE__)
#define realloc(ptr, size) debug_realloc(ptr, size, __FILE__, __LINE__)
#endif // MEM_DEBUG

void debug_start();
void debug_end();

void *debug_malloc(size_t size, const char *file, size_t line);
void debug_free(void *ptr, const char *file, size_t line);
void *debug_calloc(size_t nmemb, size_t size, const char *file, size_t line);
void *debug_realloc(void *ptr, size_t size, const char *file, size_t line);

void debug_set_out_stream(FILE *stream);

void debug_print_allocated();

#endif // mem_debug_h

