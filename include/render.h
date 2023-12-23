#ifndef RENDER_H_INCLUDED_
#define RENDER_H_INCLUDED_

#include "model.h"

typedef enum cell_color_ {
    CELL_COLOR_WHITE = 1,
    CELL_COLOR_CYAN,
    CELL_COLOR_RED,
    CELL_COLOR_YELLOW,
    CELL_COLOR_MAGNETA,
    CELL_COLOR_LEN_
} cell_color_t;

void init_renderer(void);
void render_maze(const maze_t *restrict maze);
void render_current_point(Point_t p);
void destroy_renderer(void);

#endif /* RENDER_H_INCLUDED_ */
