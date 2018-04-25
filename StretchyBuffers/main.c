#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>

// stretchy buffers invented by sean barrett

#define MAX(x, y) ((x) >= (y) ? (x) : (y))

void* xrealloc(const void *ptr, const size_t num_bytes) {
	ptr = realloc(ptr, num_bytes);
	if(!ptr) {
		perror("xrealloc failed");
		exit(1);
	}
	return ptr;
}

void* xmalloc(const size_t num_bytes) {
	void* ptr = malloc(num_bytes);
	if (!ptr) {
		perror("xmalloc failed");
		exit(1);
	}
	return ptr;
}

typedef struct BufHdr{
	size_t len;
	size_t cap;
	char buf[0]; // [0] new in C99
} BufHdr;

#define buf__hdr(b) ((BufHdr*)((char*)(b) - offsetof(BufHdr, buf)))
#define buf__fits(b, n) (buf_len(b) + (n) <= buf_cap(b))
#define buf__fit(b, n)  (buf__fits((b), (n)) ? 0 : ((b) = buf__grow((b), buf_len(b) + (n), sizeof(*(b)))))

#define buf_len(b) ((b) ? buf__hdr(b)->len : 0)
#define buf_cap(b) ((b) ? buf__hdr(b)->cap : 0)
#define buf_push(b, x) (buf__fit((b), 1), (b)[buf__hdr(b)->len++] = (x))
#define buf_free(b) ((b) ? (free(buf__hdr(b)), (b) = NULL) : 0)

const void* buf__grow(const void* buf, const size_t new_len, const size_t elem_size)
{
	auto new_cap = MAX(1 + 2 * buf_cap(buf), new_len);
	assert(new_len <= new_cap);
	auto new_size = offsetof(BufHdr, buf) + new_cap * elem_size;
	BufHdr* new_hdr;
	if (buf)
		new_hdr = xrealloc(buf__hdr(buf), new_size);
	else
	{
		new_hdr = xmalloc(new_size);
		new_hdr->len = 0;
	}
	new_hdr->cap = new_cap;
	return new_hdr->buf;
}



void list_test() {
	uint64_t *list = NULL;
	assert(list == NULL);
	assert(buf_len(list) == 0);
	const int N = 1024;
	for (int i = 0; i < N; i++)
		buf_push(list, i); //std::vector::push_back equivalent

	assert(buf_len(list) == N);
	for (uint64_t i = 0; i < buf_len(list); i++) {
		assert(list[i] == i);
	}
	buf_free(list);
	assert(list == NULL);
	assert(buf_len(list) == 0);
}

int main()
{
	list_test();
	return 0;
}
