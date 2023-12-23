#include "log_buffer.h"
#include "model.h"
#include "render.h"
#include <curses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

static maze_t maze = {.map = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                              {0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
                              {0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0},
                              {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0},
                              {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0},
                              {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0},
                              {0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0},
                              {0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0},
                              {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0},
                              {0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0},
                              {0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0},
                              {0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 0},
                              {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0},
                              {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0},
                              {0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0},
                              {0, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 0, 0},
                              {0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0},
                              {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0},
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
                              {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
                      .start = {1, 1},
                      .end = {18, 18}};

static Renderer_t renderer = {0};
static LogBuffer_t logger = {0};

static void step_callback(const maze_t *restrict maze, StateReport_t state) {
    char *msg = (char *)calloc(32, sizeof(char));
    const char *fmt = NULL;
    switch (state.state) {
    case PROGRESSING:
        fmt = "PROGESS (%d,%d)";
        break;
    case BACKTRACKING:
        fmt = "BTRACK (%d,%d)";
        break;
    case DONE:
        fmt = "DONE. EXITING";
        break;
    case FAILED:
        fmt = "FAILED. EXITING";
        break;
    default:
        fmt = "UNK (%d,%d)";
        break;
    }
    snprintf(msg, MAZER_LOGBUFFER_ENTRY_LEN, fmt, state.loc.x, state.loc.y);

    Renderer_render_maze(&renderer, maze);
    LogBuffer_insert(&logger, msg, state.state);
    Renderer_render_current_point(&renderer, state.loc);
    Renderer_render_log(&renderer, &logger);
    Renderer_commit_all(&renderer);
    getch();

    free(msg);
}

int main(void) {
    if (Renderer_init(&renderer)) {
        return 1;
    }
    LogBuffer_init(&logger);

    find_path(&maze, step_callback);

    Renderer_destroy(&renderer);
    return 0;
}
