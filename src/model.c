#include "model.h"

#include "stack.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

int find_path(const maze_t *restrict maze, const step_callback_t step_callback) {
    Stack_t stack;
    Stack_init(&stack, sizeof(Point_t));

    maze_t *maze_ = (maze_t *)malloc(sizeof(maze_t));
    memcpy(maze_, maze, sizeof(maze_t));

    const Point_t end = maze_->end;
    bool solution_found = false;

    Stack_push(&stack, &maze_->start);
    while (stack.len > 0) {
        Point_t *current = (Point_t *)Stack_peek(&stack);

        if (step_callback != NULL) {
            step_callback(maze_, current);
        }

        if (current->x == end.x && current->y == end.y) {
            solution_found = true;
            break;
        }

        maze_->map[current->x][current->y] = CELL_PATH_VISITED;

        Point_t next;
        next.x = current->x + 1;
        next.y = current->y;
        if (next.x < MAZER_MAZE_WIDTH && maze_->map[next.x][next.y] == CELL_PATH_UNVISITED) {
            Stack_push(&stack, &next);
            continue;
        }

        next.x = current->x - 1;
        next.y = current->y;
        if (next.x < MAZER_MAZE_WIDTH && maze_->map[next.x][next.y] == CELL_PATH_UNVISITED) {
            Stack_push(&stack, &next);
            continue;
        }

        next.x = current->x;
        next.y = current->y + 1;
        if (next.y < MAZER_MAZE_HEIGHT && maze_->map[next.x][next.y] == CELL_PATH_UNVISITED) {
            Stack_push(&stack, &next);
            continue;
        }

        next.x = current->x;
        next.y = current->y - 1;
        if (next.y < MAZER_MAZE_HEIGHT && maze_->map[next.x][next.y] == CELL_PATH_UNVISITED) {
            Stack_push(&stack, &next);
            continue;
        }

        Stack_pop(&stack, NULL);
    }

    Stack_destroy(&stack);
    free(maze_);

    return solution_found ? 0 : 1;
}
