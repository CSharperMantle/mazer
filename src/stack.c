#include "stack.h"

#include <stdlib.h>
#include <string.h>

static int Stack_adjust_cap_(Stack_t *restrict s) {
    if (s == NULL) {
        return 1;
    }

    if (s->cap == 0) {
        uint8_t *new_buf = (uint8_t *)malloc(STACK_INITIAL_CAP * s->elem_size);
        if (new_buf == NULL) {
            return 1;
        }
        s->buf = new_buf;
        s->cap = STACK_INITIAL_CAP;
    } else if (s->len == s->cap) {
        size_t new_cap = s->cap * 2;
        uint8_t *new_buf = (uint8_t *)realloc(s->buf, new_cap * s->elem_size);
        if (new_buf == NULL) {
            return 1;
        }
        s->buf = new_buf;
        s->cap = new_cap;
    }

    return 0;
}

int Stack_init(Stack_t *restrict s, size_t elem_size) {
    if (s == NULL || elem_size == 0) {
        return 1;
    }

    memset(s, 0, sizeof(Stack_t));
    s->elem_size = elem_size;

    return 0;
}

int Stack_push(Stack_t *restrict s, const void *restrict elem) {
    if (s == NULL || elem == NULL) {
        return 1;
    }

    Stack_adjust_cap_(s);
    memcpy(s->buf + s->len * s->elem_size, elem, s->elem_size);
    s->len++;

    return 0;
}

int Stack_pop(Stack_t *restrict s, void *restrict out_result) {
    if (s == NULL || s->len == 0) {
        return 1;
    }

    if (out_result != NULL) {
        memcpy(out_result, s->buf + (s->len - 2) * s->elem_size, s->elem_size);
    }
    s->len--;

    return 0;
}

void *Stack_peek(const Stack_t *restrict s) {
    if (s == NULL || s->len == 0) {
        return NULL;
    }

    return s->buf + (s->len - 1) * s->elem_size;
}

size_t Stack_len(const Stack_t *restrict s) {
    return s == NULL ? 0 : s->len;
}

int Stack_destroy(Stack_t *restrict s) {
    free(s->buf);
    s->buf = NULL;
    s->cap = 0;
    s->len = 0;

    return 0;
}
