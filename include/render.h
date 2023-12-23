#ifndef RENDER_H_INCLUDED_
#define RENDER_H_INCLUDED_

#include "model.h"
#include <curses.h>

#define MAZER_TERM_MIN_WIDTH 50
#define MAZER_TERM_MIN_HEIGHT 32

#define MAZER_WINDOW_LOG_WIDTH 20
#define MAZER_WINDOW_COMMAND_HEIGHT 2

#define MAZER_WINDOW_PAD 2
#define MAZER_COMMAND_VERT_GAP 1

typedef enum cell_color_ {
    CELL_COLOR_WHITE = 1,
    CELL_COLOR_CYAN,
    CELL_COLOR_RED,
    CELL_COLOR_YELLOW,
    CELL_COLOR_MAGNETA,
    CELL_COLOR_LEN_
} cell_color_t;

typedef struct Window_ {
    WINDOW *handle;
    int padding_lr;
    int padding_tb;
    int width_real;
    int height_real;
} Window_t;

void Window_init(Window_t *restrict win, int w, int h, int p_lr, int p_tb, int y, int x);
Point_t Window_pad_point(const Window_t *restrict win, Point_t p);
void Window_destroy(Window_t *restrict win);

typedef struct Renderer_ {
    Window_t win_game;
    Window_t win_log;
    Window_t win_command;
} Renderer_t;

int Renderer_init(Renderer_t *restrict r);
void Renderer_render_maze(const Renderer_t *restrict r, const maze_t *restrict maze);
void Renderer_render_current_point(const Renderer_t *restrict r, Point_t p);
void Renderer_commit_all(const Renderer_t *restrict r);
void Renderer_destroy(Renderer_t *restrict r);

#endif /* RENDER_H_INCLUDED_ */
