#include "log_buffer.h"

#include <stdbool.h>
#include <string.h>

void LogBuffer_init(LogBuffer_t *restrict b) {
    memset(b, 0, sizeof(LogBuffer_t));
}

void LogBuffer_insert(LogBuffer_t *restrict b, const char *restrict msg, int state) {
    strncpy(b->buffer[b->head].msg, msg, MAZER_LOGBUFFER_ENTRY_LEN);
    b->buffer[b->head].state = state;
    if (b->full) {
        b->tail = (b->tail + 1) % MAZER_LOGBUFFER_LEN;
    }
    b->head = (b->head + 1) % MAZER_LOGBUFFER_LEN;
    b->full = b->head == b->tail;
}
