#ifndef LIL_JVM_COMMON_H
#define LIL_JVM_COMMON_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

//#define DEBUG 0

#ifdef DEBUG
#define DEBUG_TRACE_EXECUTION
#define DEBUG_PRINT_CODE
#endif

#ifdef DEBUG_GC
#define DEBUG_STRESS_GC
#define DEBUG_LOG_GC
#endif

#define UINT8_COUNT (UINT8_MAX + 1)

// error codes
#define EXIT_SUCCESS 0
#define EXIT_MALLOC_ERROR 1
#define EXIT_MAGIC_ERROR 10
#define EXIT_FILE_ERROR 20
#define EXIT_COMPILE_ERROR 65
#define EXIT_RUNTIME_ERROR 70

#endif //LIL_JVM_COMMON_H
