#include "model.h"

#include "stack.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

int find_path(const Maze_t *restrict maze, const step_callback_t step_callback) {
    Stack_t stack;
    Stack_init(&stack, sizeof(Point_t));

    Maze_t *maze_ = (Maze_t *)malloc(sizeof(Maze_t));
    memcpy(maze_, maze, sizeof(Maze_t));

    const Point_t end = maze_->end;
    bool solution_found = false;

    progress_state_t state = PROGRESSING;
    Point_t *current = NULL;
    Stack_push(&stack, &maze_->start);
    while (stack.len > 0) {
        current = (Point_t *)Stack_peek(&stack);

        if (step_callback != NULL) {
            step_callback(maze_, (StateReport_t){
                                     .loc = *current,
                                     .state = state,
                                 });
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
            state = PROGRESSING;
            continue;
        }

        next.x = current->x - 1;
        next.y = current->y;
        if (next.x < MAZER_MAZE_WIDTH && maze_->map[next.x][next.y] == CELL_PATH_UNVISITED) {
            Stack_push(&stack, &next);
            state = PROGRESSING;
            continue;
        }

        next.x = current->x;
        next.y = current->y + 1;
        if (next.y < MAZER_MAZE_HEIGHT && maze_->map[next.x][next.y] == CELL_PATH_UNVISITED) {
            Stack_push(&stack, &next);
            state = PROGRESSING;
            continue;
        }

        next.x = current->x;
        next.y = current->y - 1;
        if (next.y < MAZER_MAZE_HEIGHT && maze_->map[next.x][next.y] == CELL_PATH_UNVISITED) {
            Stack_push(&stack, &next);
            state = PROGRESSING;
            continue;
        }

        Stack_pop(&stack, NULL);
        state = BACKTRACKING;
    }

    step_callback(maze_, (StateReport_t){
                             .loc = *current,
                             .state = solution_found ? DONE : FAILED,
                         });

    Stack_destroy(&stack);
    free(maze_);

    return solution_found ? 0 : 1;
}
