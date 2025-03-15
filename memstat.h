#include <stdint.h>

typedef struct {
	uint16_t malloc_count;
	uint16_t calloc_count;
	uint16_t realloc_count;
	uint16_t free_count;
} Memstat;

void* lika_malloc(size_t);
void* lika_calloc(size_t, size_t);
void* lika_realloc(void*, size_t);
void lika_free(void*);