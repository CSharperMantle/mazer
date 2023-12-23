#include "render.h"

#include "model.h"
#include <curses.h>
#include <string.h>

void Window_init(Window_t *restrict win, int w, int h, int p_lr, int p_tb, int y, int x) {
    memset(win, 0, sizeof(Window_t));
    win->padding_lr = p_lr;
    win->padding_tb = p_tb;
    int w_real = w + p_lr * 2;
    int h_real = h + p_tb * 2;
    WINDOW *handle = newwin(h_real, w_real, y, x);
    box(handle, 0, 0);
    wrefresh(handle);
    win->handle = handle;
    win->width_real = w_real;
    win->height_real = h_real;
}

Point_t Window_pad_point(const Window_t *restrict win, Point_t p) {
    Point_t result = {.x = p.x + win->padding_lr, .y = p.y + win->padding_tb};
    return result;
}

void Window_destroy(Window_t *restrict win) {
    WINDOW *handle = win->handle;
    wborder(handle, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(handle);
    delwin(handle);
    memset(win, 0, sizeof(Window_t));
}

static void render_point(const Window_t *restrict win, Point_t p, char ch, cell_color_t color) {
    WINDOW *handle = win->handle;
    Point_t actual_point = Window_pad_point(win, p);
    wattron(handle, COLOR_PAIR(color));
    wmove(handle, actual_point.y, actual_point.x);
    waddch(handle, ch);
    wattroff(handle, COLOR_PAIR(color));
}

int Renderer_init(Renderer_t *restrict r) {
    memset(r, 0, sizeof(Renderer_t));

    initscr();

    if (LINES < MAZER_TERM_MIN_HEIGHT || COLS < MAZER_TERM_MIN_WIDTH) {
        endwin();
        printf("Terminal size (%d:%d) too small. Minimum %d:%d\n", COLS, LINES,
               MAZER_TERM_MIN_WIDTH, MAZER_TERM_MIN_HEIGHT);
        return 1;
    }

    cbreak();
    keypad(stdscr, true);
    noecho();

    start_color();
    init_pair(CELL_COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
    init_pair(CELL_COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(CELL_COLOR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(CELL_COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(CELL_COLOR_MAGNETA, COLOR_MAGENTA, COLOR_BLACK);

    refresh();

    Window_init(&r->win_game, MAZER_MAZE_WIDTH, MAZER_MAZE_HEIGHT, MAZER_WINDOW_PAD,
                MAZER_WINDOW_PAD, 0, 0);
    Window_init(&r->win_log, MAZER_WINDOW_LOG_WIDTH, MAZER_MAZE_HEIGHT, MAZER_WINDOW_PAD,
                MAZER_WINDOW_PAD, 0, r->win_game.width_real + MAZER_WINDOW_PAD);
    Window_init(&r->win_command, r->win_game.width_real + r->win_log.width_real - MAZER_WINDOW_PAD,
                MAZER_WINDOW_COMMAND_HEIGHT, MAZER_WINDOW_PAD, MAZER_WINDOW_PAD,
                r->win_game.height_real + MAZER_COMMAND_VERT_GAP, 0);

    return 0;
}

void Renderer_render_maze(const Renderer_t *restrict r, const maze_t *restrict maze) {
    for (size_t x = 0; x < MAZER_MAZE_WIDTH; x++) {
        for (size_t y = 0; y < MAZER_MAZE_HEIGHT; y++) {
            const Point_t p = {
                .x = x,
                .y = y,
            };
            switch (maze->map[x][y]) {
            case CELL_WALL:
                render_point(&r->win_game, p, '#', CELL_COLOR_WHITE);
                break;
            case CELL_PATH_UNVISITED:
                render_point(&r->win_game, p, ' ', CELL_COLOR_WHITE);
                break;
            case CELL_PATH_VISITED:
                render_point(&r->win_game, p, '.', CELL_COLOR_CYAN);
                break;
            default:
                render_point(&r->win_game, p, '?', CELL_COLOR_WHITE);
                break;
            }
        }
    }
    render_point(&r->win_game, maze->start, 'S', CELL_COLOR_MAGNETA);
    render_point(&r->win_game, maze->end, 'E', CELL_COLOR_MAGNETA);
}

void Renderer_render_current_point(const Renderer_t *restrict r, Point_t p) {
    render_point(&r->win_game, p, '@', CELL_COLOR_YELLOW);
}

void Renderer_commit_all(const Renderer_t *restrict r) {
    wrefresh(r->win_game.handle);
    wrefresh(r->win_log.handle);
    wrefresh(r->win_command.handle);
    refresh();
}

void Renderer_destroy(Renderer_t *restrict r) {
    Window_destroy(&r->win_game);
    Window_destroy(&r->win_log);
    Window_destroy(&r->win_command);
    endwin();
    memset(r, 0, sizeof(Renderer_t));
}
