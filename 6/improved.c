#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE 1000

typedef struct {
    int capacity;
    int size;
    int element_size;
    void **data;
} Array;

Array *initArray(int element_size) {
    Array *array = malloc(sizeof(Array));

    array->capacity = 10;
    array->size = 0;
    array->element_size = element_size;
    array->data = malloc(sizeof(void*) * array->capacity);

    return array;
}

void add(Array *array, void *element) {
    if (array->size == array->capacity) {
        array->data = realloc(array->data, sizeof(void*) * (array->capacity + 10));
        array->capacity += 10;
    }
    void *new_element = malloc(array->element_size);
    memcpy(new_element, element, array->element_size);
    array->data[array->size] = new_element;
    array->size += 1;
}

void set(Array *array, int index, void *element) {
    void *new_element = malloc(array->element_size);
    memcpy(new_element, element, array->element_size);
    array->data[index] = new_element;
}

Array *copy(Array *array) {
    Array *duplicate = initArray(array->element_size);

    for (int i = 0; i < array->size; i++) {
        add(duplicate, array->data[i]);
    }

    return duplicate;
}

void delete(Array *array, int index) {
    for (int i = index; i < array->size; i++) {
        array->data[i] = array->data[i + 1];
    }

    array->size--;
}

void freeArray(Array *array) {
    for (int i = 0; i < array->size; i++) {
        free(array->data[i]);
    }
    free(array->data);
    free(array);
}

typedef struct {
    Array **data;
    int capacity;
    int element_size;
} HashMap;

HashMap *initHashMap(int element_size, int capacity) {
    HashMap *map = malloc(sizeof(HashMap));

    map->capacity = capacity;
    map->data = malloc(sizeof(Array*) * capacity);
    map->element_size = element_size;

    for (int i = 0; i < capacity; i++) {
        map->data[i] = initArray(element_size);
    }

    return map;
}

typedef struct {
    int x;
    int y;
    int dir_x;
    int dir_y;
} Position;

int compare_positions(Position* a, Position* b) {
    if (a->x != b->x) {
        return 0;
    }
    if (a->y != b->y) {
        return 0;
    }
    if (a->dir_x != b->dir_x) {
        return 0;
    }
    if (a->dir_y != b->dir_y) {
        return 0;
    }
    return 1;
}

int get_position_hash(Position* position, int capacity) {
    return (position->x * 2 + position->y) % capacity;
}

void save_position(HashMap* map, Position* position) {
    int hash = get_position_hash(position, map->capacity);

    Array *cell = map->data[hash];

    for (int i = 0; i < cell->size; i++) {
        if (compare_positions(position, (Position*) cell->data[i]) == 1) {
            return;
        }
    }

    add(cell, position);
}

int contains_position(HashMap* map, Position* position) {
    int hash = get_position_hash(position, map->capacity);

    Array *cell = map->data[hash];

    for (int i = 0; i < cell->size; i++) {
        if (compare_positions(position, (Position*) cell->data[i]) == 1) {
            return 1;
        }
    }

    return 0;
}

void free_positions(HashMap *map) {
    for (int i = 0; i < map->capacity; i++) {
        Array *cell = map->data[i];
        for (int j = 0; j < cell->size; j++) {
            free(cell->data[j]);
        }
        free(cell->data);
        free(cell);
    }

    free(map->data);
    free(map);
}

int main(int argc, char** argv) {
    char buffer[BUFFERSIZE];
    char *pointer = buffer;

    Array *map = initArray(sizeof(Array));
    while (fgets(buffer, BUFFERSIZE, stdin)) {
        pointer = buffer;
        if (*pointer == '\n') {
            break;
        }
        Array *row = initArray(sizeof(int));
        while (*pointer != '\n') {
            add(row, pointer);
            pointer++;
        }
        add(map, row);
    }

    int guard_x, guard_y, dir_x, dir_y;
    for (int y = 0; y < map->size; y++) {
        Array *row = (Array*) map->data[y];
        for (int x = 0; x < row->size; x++) {
            if (*((char*) row->data[x]) == '^') {
                guard_x = x;
                guard_y = y;
                dir_y = -1;
                dir_x = 0;
            }
        }
    }

    Array *positions = initArray(sizeof(Array));
    for (int i = 0; i < map->size; i++) {
        add(positions, initArray(sizeof(int)));
        for (int j = 0; j < map->size; j++) {
            int temp = 0;
            if ((guard_x == j) && (guard_y == i)) {
                temp = 1;
            }
            add((Array*) positions->data[i], &temp);
        }
    }
    int next_guard_x, next_guard_y;
    while (1) {
        for (int k = 0; k < 4; k++) {
            next_guard_x = guard_x + dir_x;
            next_guard_y = guard_y + dir_y;
            if ((next_guard_y >= map->size) || (next_guard_y < 0)) {
                break;
            }
            Array *row = (Array*) map->data[next_guard_y];
            if ((next_guard_x >= row->size) || (next_guard_x < 0)) {
                break;
            }
            if (*((char*) row->data[next_guard_x]) == '#') {
                if (dir_x == 1) {
                    dir_x = 0;
                    dir_y = 1;
                } else if (dir_x == -1) {
                    dir_x = 0;
                    dir_y = -1;
                } else if (dir_y == 1) {
                    dir_x = -1;
                    dir_y = 0;
                } else if (dir_y == -1) {
                    dir_x = 1;
                    dir_y = 0;
                }
            } else {
                break;
            }
        }

        next_guard_x = guard_x + dir_x;
        next_guard_y = guard_y + dir_y;
        if ((next_guard_y >= map->size) || (next_guard_y < 0)) {
            break;
        }
        Array *row = (Array*) map->data[next_guard_y];
        if ((next_guard_x >= row->size) || (next_guard_x < 0)) {
            break;
        }

        guard_x = next_guard_x;
        guard_y = next_guard_y;

        Array *positions_row = (Array*) positions->data[guard_y];
        int temp = 1;
        set(positions_row, guard_x, &temp);
    }

    int result = 0;
    for (int y = 0; y < map->size; y++) {
        Array *row = (Array*) positions->data[y];
        for (int x = 0; x < row->size; x++) {
            if (*((int*) row->data[x]) == 1) {
                result++;
            }
        }
    }

    Array* new_map = initArray(sizeof(Array));
    for (int i = 0; i < map->size; i++) {
        add(new_map, copy((Array*) map->data[i]));
    }

    int original_guard_x, original_guard_y;
    for (int y = 0; y < map->size; y++) {
        Array *row = (Array*) map->data[y];
        for (int x = 0; x < row->size; x++) {
            if (*((char*) row->data[x]) == '^') {
                original_guard_x = x;
                original_guard_y = y;
                dir_y = -1;
                dir_x = 0;
            }
        }
    }

    int valid_obs_count = 0;
    for (int i = 0; i < new_map->size; i++) {
        printf("%d/%d\n", i, new_map->size);
        Array *row = (Array*) new_map->data[i];
        for (int j = 0; j < row->size; j++) {
            HashMap *past_positions = initHashMap(sizeof(Position), map->size * row->size);
            guard_x = original_guard_x;
            guard_y = original_guard_y;
            dir_y = -1;
            dir_x = 0;
            if (*((char*) row->data[j]) == '#') {
                continue;
            }
            if (*((char*) row->data[j]) == '^') {
                continue;
            }
            char temp = '#';
            set(row, j, &temp);
            int valid_obs = 0;
            while (1) {
                for (int k = 0; k < 4; k++) {
                    next_guard_x = guard_x + dir_x;
                    next_guard_y = guard_y + dir_y;
                    if ((next_guard_y >= new_map->size) || (next_guard_y < 0)) {
                        break;
                    }
                    Array *row = (Array*) new_map->data[next_guard_y];
                    if ((next_guard_x >= row->size) || (next_guard_x < 0)) {
                        break;
                    }
                    if (*((char*) row->data[next_guard_x]) == '#') {
                        if (dir_x == 1) {
                            dir_x = 0;
                            dir_y = 1;
                        } else if (dir_x == -1) {
                            dir_x = 0;
                            dir_y = -1;
                        } else if (dir_y == 1) {
                            dir_x = -1;
                            dir_y = 0;
                        } else if (dir_y == -1) {
                            dir_x = 1;
                            dir_y = 0;
                        }
                    } else {
                        break;
                    }
                }

                next_guard_x = guard_x + dir_x;
                next_guard_y = guard_y + dir_y;
                if ((next_guard_y >= new_map->size) || (next_guard_y < 0)) {
                    break;
                }
                Array *row = (Array*) new_map->data[next_guard_y];
                if ((next_guard_x >= row->size) || (next_guard_x < 0)) {
                    break;
                }

                guard_x = next_guard_x;
                guard_y = next_guard_y;

                Position *new_position = malloc(sizeof(Position));
                new_position->x = guard_x;
                new_position->y = guard_y;
                new_position->dir_x = dir_x;
                new_position->dir_y = dir_y;

                valid_obs = contains_position(past_positions, new_position);
                if (valid_obs == 1) {
                    free(new_position);
                    break;
                }

                save_position(past_positions, new_position);
                free(new_position);
            }
            if (valid_obs == 1) {
                valid_obs_count++;
            }
            row = (Array*) new_map->data[i];
            temp = '.';
            set(row, j, &temp);

            free_positions(past_positions);
        }
    }

    printf("%d\n", result);
    printf("%d\n", valid_obs_count);
    return 0;
}
