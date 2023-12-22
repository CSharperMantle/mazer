#ifndef MODEL_H_INCLUDED_
#define MODEL_H_INCLUDED_

#include <stdbool.h>

#define MAZE_HEIGHT 10
#define MAZE_WIDTH 10

typedef enum direction_ {
    LEFT,
    UP,
    RIGHT,
    DOWN,
    DIRECTION_LEN_,
} direction_t;

typedef enum cell_type_ {
    CELL_WALL,
    CELL_PATH,
    CELL_TYPE_LEN_,
} cell_type_t;

typedef struct Point_ {
    int x;
    int y;
} Point_t;

typedef struct StepState_ {
    bool children[DIRECTION_LEN_];
} StepState_t;

#endif /* MODEL_H_INCLUDED_ */
