#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BUFFERSIZE 100000

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

int mapSize(HashMap *map) {
    int result = 0;
    for (int i = 0; i < map->capacity; i++) {
        result += ((Array*) map->data[i])->size;
    }
    return result;
}

typedef struct {
    long x;
    long y;
} Position;

int comparePosition(Position *a, Position *b) {
    if (a->x != b->x) {
        return 0;
    }
    if (a->y != b->y) {
        return 0;
    }
    return 1;
}

void addPosition(HashMap *map, Position* position) {
    int hash = (position->x * position->y) % map->capacity;

    Array *array = (Array*) map->data[hash];

    for (int i = 0; i < array->size; i++) {
        Position *saved_position = (Position*) array->data[i];
        if (comparePosition(position, saved_position) == 1) {
            return;
        }
    }
    add(array, position);
}

int containsPosition(HashMap *map, Position *position) {
    int hash = (position->x * position->y) % map->capacity;

    Array *array = (Array*) map->data[hash];

    for (int i = 0; i < array->size; i++) {
        Position *saved_position = (Position*) array->data[i];
        if (comparePosition(position, saved_position) == 1) {
            return 1;
        }
    }
    return 0;
}

Array *getPositions(HashMap *map) {
    Array *result = initArray(sizeof(Position));
    for (int i = 0; i < map->capacity; i++) {
        Array *array = map->data[i];
        for (int j = 0; j < array->size; j++) {
            add(result, (Position*) array->data[j]);
        }
    }
    return result;
}

void freeHashMap(HashMap *map) {
    for (int i = 0; i < map->capacity; i++) {
        Array *array = (Array*) map->data[i];
        for (int j = 0; j < array->size; j++) {
            free(array->data[j]);
        }
        free(array->data);
        free(array);
    }
    free(map->data);
    free(map);
}

int can_move(Array *map, Position *p, int dir_x, int dir_y) {
    int new_x = p->x + dir_x;
    int new_y = p->y + dir_y;

    while (1) {
        if (new_y >= map->size) {
            return 0;
        }

        Array *row = (Array*) map->data[new_y];

        if (new_x >= row->size) {
            return 0;
        }

        char cell = *((char*) row->data[new_x]);

        if (cell == '#') {
            return 0;
        }

        if (cell == '.') {
            return 1;
        }

        new_x += dir_x;
        new_y += dir_y;
    }

    return 0;
}

int can_move_2(Array *map, int x, int y, int dir_x, int dir_y) {
    if (y >= map->size) {
        return 0;
    }

    Array *row = (Array*) map->data[y];

    if (x >= row->size) {
        return 0;
    }

    char cell = *((char*) row->data[x]);

    if (cell == '#') {
        return 0;
    }
    if (cell == '.') {
        return 1;
    }

    if (dir_y == 0) {
        int can_move_center = can_move_2(map, x+dir_x, y+dir_y, dir_x, dir_y);
        return can_move_center;
    }

    if (cell == '[') {
        int can_move_center = can_move_2(map, x+dir_x, y+dir_y, dir_x, dir_y);
        int can_move_right = can_move_2(map, x+1+dir_x, y+dir_y, dir_x, dir_y);
        if (can_move_center != 1) {
            return 0;
        }
        if (can_move_right != 1) {
            return 0;
        }
        return 1;
    }
    if (cell == ']') {
        int can_move_left = can_move_2(map, x-1+dir_x, y+dir_y, dir_x, dir_y);
        int can_move_center = can_move_2(map, x+dir_x, y+dir_y, dir_x, dir_y);
        if (can_move_center != 1) {
            return 0;
        }
        if (can_move_left != 1) {
            return 0;
        }
        return 1;
    }

    return can_move_2(map, x+dir_x, y+dir_y, dir_x, dir_y);
}

void do_move_2(Array *map, char prev, int x, int y, int dir_x, int dir_y) {
    if (y >= map->size) {
        return;
    }

    Array *row = (Array*) map->data[y];

    if (x >= row->size) {
        return;
    }

    char cell = *((char*) row->data[x]);

    if (cell == '#') {
        return;
    }
    if (cell == '.') {
        set(row, x, &prev);
        return;
    }

    if (dir_y == 0) {
        do_move_2(map, cell, x+dir_x, y+dir_y, dir_x, dir_y);
        set(row, x, &prev);
        return;
    }

    if (cell == '[') {
        do_move_2(map, cell, x+dir_x, y+dir_y, dir_x, dir_y);
        do_move_2(map, ']', x+1+dir_x, y+dir_y, dir_x, dir_y);
        set(row, x, &prev);
        char temp = '.';
        set(row, x+1, &temp);
        return;
    }
    if (cell == ']') {
        do_move_2(map, cell, x+dir_x, y+dir_y, dir_x, dir_y);
        do_move_2(map, '[', x-1+dir_x, y+dir_y, dir_x, dir_y);
        set(row, x, &prev);
        char temp = '.';
        set(row, x-1, &temp);
        return;
    }

    do_move_2(map, cell, x+dir_x, y+dir_y, dir_x, dir_y);
    set(row, x, &prev);
    return;
}

void do_move(Array *map, Position *p, int dir_x, int dir_y) {
    char temp_prev = '@';
    char temp = '.';
    Array *row = (Array*) map->data[p->y];
    char cell = *((char*) row->data[p->x]);
    temp_prev = cell;

    set(row, p->x, &temp);
    temp = temp_prev;

    int new_x = p->x + dir_x;
    int new_y = p->y + dir_y;

    while (1) {
        if (new_y >= map->size) {
            return;
        }

        row = (Array*) map->data[new_y];

        if (new_x >= row->size) {
            return;
        }

        cell = *((char*) row->data[new_x]);

        if (cell == '#') {
            return;
        }
        temp_prev = cell;
        set(row, new_x, &temp);

        if (cell == '.') {
            return;
        }

        new_x += dir_x;
        new_y += dir_y;
        temp = temp_prev;
    }

    return;
}

void simulate_robot(Array *map, Position *p, char move) {
    int dir_x = 0;
    int dir_y = 0;

    if (move == '^') {
        dir_x = 0;
        dir_y = -1;
    } else if (move == '<') {
        dir_x = -1;
        dir_y = 0;
    } else if (move == '>') {
        dir_x = 1;
        dir_y = 0;
    } else if (move == 'v') {
        dir_x = 0;
        dir_y = 1;
    }

    if (can_move(map, p, dir_x, dir_y) == 0) {
        return;
    }

    do_move(map, p, dir_x, dir_y);

    p->x += dir_x;
    p->y += dir_y;
}

void simulate_robot_2(Array *map, Position *p, char move) {
    int dir_x = 0;
    int dir_y = 0;

    if (move == '^') {
        dir_x = 0;
        dir_y = -1;
    } else if (move == '<') {
        dir_x = -1;
        dir_y = 0;
    } else if (move == '>') {
        dir_x = 1;
        dir_y = 0;
    } else if (move == 'v') {
        dir_x = 0;
        dir_y = 1;
    }

    if (can_move_2(map, p->x, p->y, dir_x, dir_y) == 0) {
        return;
    }

    do_move_2(map, '.', p->x, p->y, dir_x, dir_y);

    p->x += dir_x;
    p->y += dir_y;
}

void print_map(Array *map) {
    for (int y = 0; y < map->size; y++) {
        Array *row = (Array*) map->data[y];
        for (int x = 0; x < row->size; x++) {
            char cell = *((char*) row->data[x]);
            printf("%c", cell);
        }
        printf("\n");
    }
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

        Array *row = initArray(sizeof(char));
        while (*pointer != '\n') {
            add(row, pointer);
            pointer++;
        }
        add(map, row);
    }

    Array *map_2 = initArray(sizeof(Array));
    for (int y = 0; y < map->size; y++) {
        Array *row_2 = initArray(sizeof(char));
        Array *row = (Array*) map->data[y];
        for (int x = 0; x < row->size; x++) {
            char cell = *((char*) row->data[x]);
            if (cell == '#') {
                char temp = '#';
                add(row_2, &temp);
                add(row_2, &temp);
                continue;
            }
            if (cell == 'O') {
                char temp = '[';
                add(row_2, &temp);
                temp = ']';
                add(row_2, &temp);
                continue;
            }
            if (cell == '.') {
                char temp = '.';
                add(row_2, &temp);
                add(row_2, &temp);
                continue;
            }
            if (cell == '@') {
                char temp = '@';
                add(row_2, &temp);
                temp = '.';
                add(row_2, &temp);
                continue;
            }
        }
        add(map_2, row_2);
    }

    Array *moves = initArray(sizeof(char));
    while (fgets(buffer, BUFFERSIZE, stdin)) {
        pointer = buffer;
        if (*pointer == '\n') {
            break;
        }

        while (*pointer != '\n') {
            add(moves, pointer);
            pointer++;
        }
    }

    Position robot_position;
    for (int y = 0; y < map->size; y++) {
        Array *row = (Array*) map->data[y];
        for (int x = 0; x < row->size; x++) {
            char cell = *((char*) row->data[x]);
            if (cell == '@') {
                robot_position.x = x;
                robot_position.y = y;
                break;
            }
        }
    }

    for (int i = 0; i < moves->size; i++) {
        char move = *((char*) moves->data[i]);
        simulate_robot(map, &robot_position, move);
    }

    robot_position;
    for (int y = 0; y < map_2->size; y++) {
        Array *row_2 = (Array*) map_2->data[y];
        for (int x = 0; x < row_2->size; x++) {
            char cell = *((char*) row_2->data[x]);
            if (cell == '@') {
                robot_position.x = x;
                robot_position.y = y;
                break;
            }
        }
    }

    for (int i = 0; i < moves->size; i++) {
        char move = *((char*) moves->data[i]);
        simulate_robot_2(map_2, &robot_position, move);
    }

    long total_gps = 0;
    for (int y = 0; y < map->size; y++) {
        Array *row = (Array*) map->data[y];
        for (int x = 0; x < row->size; x++) {
            char cell = *((char*) row->data[x]);
            if (cell == 'O') {
                total_gps += 100 * y + x;
            }
        }
    }

    long total_gps_2 = 0;
    for (int y = 0; y < map_2->size; y++) {
        Array *row = (Array*) map_2->data[y];
        for (int x = 0; x < row->size; x++) {
            char cell = *((char*) row->data[x]);
            if (cell == '[') {
                total_gps_2 += 100 * y + x;
            }
        }
    }

    printf("%lld\n", total_gps);
    printf("%lld\n", total_gps_2);

    return 0;
}
