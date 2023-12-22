#include "render.h"

#include <curses.h>

void render_maze(const cell_type_t maze[MAZE_WIDTH][MAZE_HEIGHT]) {
    for (size_t y = 0; y < MAZE_HEIGHT; y++) {
        for (size_t x = 0; x < MAZE_WIDTH; x++) {
            move(y, x);
            switch (maze[x][y]) {
            case CELL_WALL:
                addch('#');
                break;
            case CELL_PATH:
                addch(' ');
                break;
            default:
                addch('?');
                break;
            }
        }
    }
}
