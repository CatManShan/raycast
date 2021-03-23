#ifndef mem_macros_h
#define mem_macros_h

#define ALLOC_ARR(arr, size) malloc(size * sizeof arr[0])
#define REALLOC_ARR(arr, size) realloc(arr, size * sizeof arr[0])

#define CALLOC_ARR(arr, size) calloc(size, sizeof arr[0])

#define ALLOC_FLEX_STRUCT(struc, arr, size) malloc(sizeof *struc + size * sizeof struc->arr[0])
#define REALLOC_FLEX_STRUCT(struc, arr, size) realloc(struc, sizeof *struc + size * sizeof struc->arr[0])

#define ALLOC_STR_LENGTH(length) malloc(length + 1)
#define ALLOC_STR_SIZE(size) malloc(size)

#endif // mem_macros_h

