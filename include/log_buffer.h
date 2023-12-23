#ifndef LOG_BUFFER_H_INCLUDED_
#define LOG_BUFFER_H_INCLUDED_

#include <stdbool.h>

#define MAZER_LOGBUFFER_LEN 20
#define MAZER_LOGBUFFER_ENTRY_LEN 20

typedef struct LogBufferNode_ {
    char msg[MAZER_LOGBUFFER_ENTRY_LEN + 1];
    int state;
} LogBufferNode_t;

typedef struct LogBuffer_ {
    LogBufferNode_t buffer[MAZER_LOGBUFFER_LEN];
    bool full;
    int head;
    int tail;
} LogBuffer_t;

void LogBuffer_init(LogBuffer_t *restrict b);
void LogBuffer_insert(LogBuffer_t *restrict b, const char *restrict msg, int state);

#endif /* LOG_BUFFER_H_INCLUDED_ */
