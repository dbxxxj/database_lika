#include <stdlib.h>
#include <stdint.h>
#include "memstat.h"

Memstat memstat;


void* lika_malloc(size_t size) {
	memstat.malloc_count++;
	return malloc(size);
}

void* lika_calloc(size_t num, size_t size) {
	memstat.calloc_count++;
	return calloc(num, size);
}

void* lika_realloc(void* pointer, size_t size) {
	memstat.realloc_count++;
	return realloc(pointer, size);
}

void lika_free(void* pointer) {
	memstat.free_count++;
	free(pointer);
}


