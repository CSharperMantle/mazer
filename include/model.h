#ifndef MODEL_H_INCLUDED_
#define MODEL_H_INCLUDED_

#include <stdbool.h>

#define MAZER_MAZE_HEIGHT 20
#define MAZER_MAZE_WIDTH 20

typedef enum direction_ {
    LEFT,
    UP,
    RIGHT,
    DOWN,
    DIRECTION_LEN_,
} direction_t;

typedef enum cell_type_ {
    CELL_WALL,
    CELL_PATH_UNVISITED,
    CELL_PATH_VISITED,
    CELL_TYPE_LEN_,
} cell_type_t;

typedef struct Point_ {
    int x;
    int y;
} Point_t;

typedef struct maze_ {
    cell_type_t map[MAZER_MAZE_WIDTH][MAZER_MAZE_HEIGHT];
    Point_t start;
    Point_t end;
} maze_t;

typedef void (*step_callback_t)(const maze_t *restrict maze, const Point_t *restrict current);

int find_path(const maze_t *restrict maze, const step_callback_t step_callback);

#endif /* MODEL_H_INCLUDED_ */
