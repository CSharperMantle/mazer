#include "render.h"

#include "model.h"
#include <curses.h>

void init_renderer(void) {
    initscr();

    raw();
    keypad(stdscr, true);
    noecho();

    start_color();
    init_pair(CELL_COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
    init_pair(CELL_COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(CELL_COLOR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(CELL_COLOR_YELLOW, COLOR_GREEN, COLOR_BLACK);
}

void render_maze(const maze_t maze) {
    for (size_t x = 0; x < MAZE_WIDTH; x++) {
        for (size_t y = 0; y < MAZE_HEIGHT; y++) {
            move(y, x);
            switch (maze[x][y]) {
            case CELL_WALL:
                attron(COLOR_PAIR(CELL_COLOR_WHITE));
                addch('#');
                attroff(COLOR_PAIR(CELL_COLOR_WHITE));
                break;
            case CELL_PATH_UNVISITED:
                addch(' ');
                break;
            case CELL_PATH_VISITED:
                attron(COLOR_PAIR(CELL_COLOR_CYAN));
                addch('.');
                attroff(COLOR_PAIR(CELL_COLOR_CYAN));
                break;
            default:
                addch('?');
                break;
            }
        }
    }
}

void render_current_point(const Point_t p) {
    attron(COLOR_PAIR(CELL_COLOR_YELLOW));
    mvaddch(p.y, p.x, '@');
    attroff(COLOR_PAIR(CELL_COLOR_YELLOW));
}

void destroy_renderer(void) {
    endwin();
}
