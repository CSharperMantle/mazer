#include "render.h"

#include "log_buffer.h"
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
    Window_commit(win);
    win->handle = handle;
    win->width_real = w_real;
    win->height_real = h_real;
}

Point_t Window_pad_point(const Window_t *restrict win, Point_t p) {
    Point_t result = {.x = p.x + win->padding_lr, .y = p.y + win->padding_tb};
    return result;
}

void Window_commit(const Window_t *restrict win) {
    wmove(win->handle, 0, 0);
    wrefresh(win->handle);
}

void Window_destroy(Window_t *restrict win) {
    WINDOW *handle = win->handle;
    wborder(handle, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    Window_commit(win);
    delwin(handle);
    memset(win, 0, sizeof(Window_t));
}

static void render_point(const Window_t *restrict win, Point_t p, char ch, cell_color_t color) {
    WINDOW *handle = win->handle;
    p = Window_pad_point(win, p);
    wattron(handle, COLOR_PAIR(color));
    mvwaddch(handle, p.y, p.x, ch);
    wattroff(handle, COLOR_PAIR(color));
}

static void render_text(const Window_t *restrict win, Point_t p, const char *text,
                        cell_color_t color, int clear_len) {
    WINDOW *handle = win->handle;
    p = Window_pad_point(win, p);
    for (int i = 0; i < clear_len; i++) {
        mvwaddch(handle, p.y, p.x + i, ' ');
    }
    wattron(handle, COLOR_PAIR(color));
    mvwprintw(handle, p.y, p.x, "%s", text);
    wattroff(handle, COLOR_PAIR(color));
}

static cell_color_t state_to_color(progress_state_t state) {
    switch (state) {
    case PROGRESSING:
        return CELL_COLOR_CYAN;
    case BACKTRACKING:
        return CELL_COLOR_YELLOW;
    case DONE:
        return CELL_COLOR_GREEN;
    case FAILED:
        return CELL_COLOR_RED;
    default:
        return CELL_COLOR_WHITE;
    }
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
    init_pair(CELL_COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(CELL_COLOR_HIGHLIGHT, COLOR_BLACK, COLOR_WHITE);

    refresh();

    Window_init(&r->win_game, MAZER_MAZE_WIDTH, MAZER_MAZE_HEIGHT, MAZER_WINDOW_PAD,
                MAZER_WINDOW_PAD, 0, 0);
    Window_init(&r->win_log, MAZER_WINDOW_LOG_WIDTH, MAZER_MAZE_HEIGHT, MAZER_WINDOW_PAD,
                MAZER_WINDOW_PAD, 0, r->win_game.width_real + MAZER_WINDOW_PAD);
    Window_init(&r->win_command, r->win_game.width_real + r->win_log.width_real - MAZER_WINDOW_PAD,
                MAZER_WINDOW_COMMAND_HEIGHT, MAZER_WINDOW_PAD, MAZER_COMMAND_PAD,
                r->win_game.height_real + MAZER_COMMAND_VERT_GAP, 0);

    return 0;
}

void Renderer_render_maze(const Renderer_t *restrict r, const Maze_t *restrict maze) {
    for (int x = 0; x < MAZER_MAZE_WIDTH; x++) {
        for (int y = 0; y < MAZER_MAZE_HEIGHT; y++) {
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
    Window_commit(&r->win_game);
}

void Renderer_render_maze_highlight(const Renderer_t *restrict r, const Maze_t *restrict maze,
                                    Point_t highlight) {
    for (int x = 0; x < MAZER_MAZE_WIDTH; x++) {
        for (int y = 0; y < MAZER_MAZE_HEIGHT; y++) {
            bool is_highlight = x == highlight.x && y == highlight.y;
            const Point_t p = {
                .x = x,
                .y = y,
            };
            switch (maze->map[x][y]) {
            case CELL_WALL:
                render_point(&r->win_game, p, '#',
                             is_highlight ? CELL_COLOR_HIGHLIGHT : CELL_COLOR_WHITE);
                break;
            case CELL_PATH_UNVISITED:
                render_point(&r->win_game, p, ' ',
                             is_highlight ? CELL_COLOR_HIGHLIGHT : CELL_COLOR_WHITE);
                break;
            case CELL_PATH_VISITED:
                render_point(&r->win_game, p, '.',
                             is_highlight ? CELL_COLOR_HIGHLIGHT : CELL_COLOR_CYAN);
                break;
            default:
                render_point(&r->win_game, p, '?',
                             is_highlight ? CELL_COLOR_HIGHLIGHT : CELL_COLOR_WHITE);
                break;
            }
        }
    }
    render_point(&r->win_game, maze->start, 'S', CELL_COLOR_MAGNETA);
    render_point(&r->win_game, maze->end, 'E', CELL_COLOR_MAGNETA);
    Window_commit(&r->win_game);
}

void Renderer_render_current_point(const Renderer_t *restrict r, Point_t p) {
    render_point(&r->win_game, p, '@', CELL_COLOR_YELLOW);
    Window_commit(&r->win_game);
}

void Renderer_clear_log(const Renderer_t *restrict r) {
    for (int i = 0; i < MAZER_LOGBUFFER_LEN; i++) {
        Point_t p = {
            .x = 0,
            .y = i,
        };
        render_text(&r->win_log, p, "", CELL_COLOR_WHITE, MAZER_LOGBUFFER_ENTRY_LEN);
    }
}

void Renderer_render_log(const Renderer_t *restrict r, const LogBuffer_t *restrict buf) {
    if (buf->full) {
        int ln = 0;
        for (int i = buf->tail; i < MAZER_LOGBUFFER_LEN; i++) {
            Point_t p = {
                .x = 0,
                .y = ln,
            };
            const LogBufferNode_t *entry = buf->buffer + i;
            render_text(&r->win_log, p, entry->msg, state_to_color(entry->state),
                        MAZER_LOGBUFFER_ENTRY_LEN);
            ln++;
        }
        for (int i = 0; i < buf->head; i++) {
            Point_t p = {
                .x = 0,
                .y = ln,
            };
            const LogBufferNode_t *entry = buf->buffer + i;
            render_text(&r->win_log, p, entry->msg, state_to_color(entry->state),
                        MAZER_LOGBUFFER_ENTRY_LEN);
            ln++;
        }
    } else {
        int ln = 0;
        for (int i = buf->tail; i < buf->head; i++) {
            Point_t p = {
                .x = 0,
                .y = ln,
            };
            const LogBufferNode_t *entry = buf->buffer + i;
            render_text(&r->win_log, p, entry->msg, state_to_color(entry->state),
                        MAZER_LOGBUFFER_ENTRY_LEN);
            ln++;
        }
    }
    Window_commit(&r->win_log);
}

void Renderer_render_command(const Renderer_t *restrict r, const char *restrict buf,
                             cell_color_t color) {
    WINDOW *handle = r->win_command.handle;
    int width_content = r->win_command.width_real - 2 * r->win_command.padding_lr;
    for (int i = 0; i < width_content; i++) {
        Point_t p = Window_pad_point(&r->win_command, (Point_t){
                                                          .x = i,
                                                          .y = 0,
                                                      });
        mvwaddch(handle, p.y, p.x, ' ');
    }

    Point_t p = Window_pad_point(&r->win_command, (Point_t){
                                                      .x = 0,
                                                      .y = 0,
                                                  });
    wattron(handle, COLOR_PAIR(color));
    mvwprintw(handle, p.y, p.x, "%s", buf);
    wattroff(handle, COLOR_PAIR(color));
    Window_commit(&r->win_command);
}

void Renderer_commit_all(const Renderer_t *restrict r) {
    Window_commit(&r->win_game);
    Window_commit(&r->win_log);
    Window_commit(&r->win_command);
    wrefresh(stdscr);
}

void Renderer_destroy(Renderer_t *restrict r) {
    Window_destroy(&r->win_game);
    Window_destroy(&r->win_log);
    Window_destroy(&r->win_command);
    endwin();
    memset(r, 0, sizeof(Renderer_t));
}
