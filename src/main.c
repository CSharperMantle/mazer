#include "log_buffer.h"
#include "model.h"
#include "render.h"
#include <curses.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAZER_COMMAND_MAX_LEN 40

static Maze_t maze = {.map = {{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
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

static void step_callback(const Maze_t *restrict maze, StateReport_t state) {
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
        fmt = "DONE";
        break;
    case FAILED:
        fmt = "FAILED";
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

    free(msg);

    getch();
}

int main_loop(void) {
    LogBuffer_init(&logger);
    Renderer_clear_log(&renderer);
    Renderer_commit_all(&renderer);

    Renderer_render_maze(&renderer, &maze);
    Renderer_render_command(&renderer, "EDIT: q=run; hjkl=move; Spc=cycle; se=terms",
                            CELL_COLOR_WHITE);

    Point_t cursor = {0};
    while (true) {
        int ch = getch();
        if (ch == 'q' || ch == 'Q') {
            break;
        }
        switch (ch) {
        case 'h':
            cursor.x--;
            break;
        case 'j':
            cursor.y++;
            break;
        case 'k':
            cursor.y--;
            break;
        case 'l':
            cursor.x++;
            break;
        case ' ':
            maze.map[cursor.x][cursor.y] =
                maze.map[cursor.x][cursor.y] == CELL_WALL ? CELL_PATH_UNVISITED : CELL_WALL;
            break;
        case 's':
            maze.start = cursor;
            break;
        case 'e':
            maze.end = cursor;
            break;
        default:
            break;
        }
        cursor.x = cursor.x < 0 ? 0 : cursor.x;
        cursor.x = cursor.x >= MAZER_MAZE_WIDTH ? MAZER_MAZE_WIDTH - 1 : cursor.x;
        cursor.y = cursor.y < 0 ? 0 : cursor.y;
        cursor.y = cursor.y >= MAZER_MAZE_HEIGHT ? MAZER_MAZE_HEIGHT - 1 : cursor.y;
        Renderer_render_maze_highlight(&renderer, &maze, cursor);
    }

    Renderer_render_command(&renderer, "RUN: Ctrl-C - quit, others - step", CELL_COLOR_WHITE);
    find_path(&maze, step_callback);

    return 0;
}

int main(void) {
    if (Renderer_init(&renderer)) {
        return 1;
    }
    while (!main_loop()) {
        ;
    }
    Renderer_destroy(&renderer);
    return 0;
}
