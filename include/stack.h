#ifndef STACK_H_INCLUDED_
#define STACK_H_INCLUDED_

#include <stdint.h>
#include <stdlib.h>

#define STACK_INITIAL_CAP 16

typedef struct Stack_ {
    uint8_t *buf;
    size_t len;
    size_t cap;
    size_t elem_size;
} Stack_t;

int Stack_init(Stack_t *restrict s, size_t elem_size);
int Stack_push(Stack_t *restrict s, void *restrict elem);
int Stack_pop(Stack_t *restrict s, void *restrict out_result);
void *Stack_peek(Stack_t *restrict s);
size_t Stack_len(Stack_t *restrict s);
int Stack_destroy(Stack_t *restrict s);

#endif /* STACK_H_INCLUDED_ */
