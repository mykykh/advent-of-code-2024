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

int mapSize(HashMap *map) {
    int result = 0;
    for (int i = 0; i < map->capacity; i++) {
        result += ((Array*) map->data[i])->size;
    }
    return result;
}

typedef struct {
    int x;
    int y;
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
    int hash = (position->x * 2 + position->y) % map->capacity;

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
    int hash = (position->x * 2 + position->y) % map->capacity;

    Array *array = (Array*) map->data[hash];

    for (int i = 0; i < array->size; i++) {
        Position *saved_position = (Position*) array->data[i];
        if (comparePosition(position, saved_position) == 1) {
            return 1;
        }
    }
    return 0;
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

int count_trailheads(Array *map, int x, int y, int previous_height, HashMap *result_positions) {
    if (y < 0 || y >= map->size) {
        return 0;
    }
    Array *row = (Array*) map->data[y];
    if (x < 0 || x >= row->size) {
        return 0;
    }
    int current_height = *((int*) row->data[x]);
    if (current_height != previous_height + 1) {
        return 0;
    }
    if (current_height == 9) {
        Position position;
        position.x = x;
        position.y = y;
        if (containsPosition(result_positions, &position) == 0) {
            addPosition(result_positions, &position);
        }
        return 1;
    }
    int result = 0;
    result += count_trailheads(map, x, y - 1, current_height, result_positions);
    result += count_trailheads(map, x - 1, y, current_height, result_positions);
    result += count_trailheads(map, x, y + 1, current_height, result_positions);
    result += count_trailheads(map, x + 1, y, current_height, result_positions);
    return result;
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
            int height = (*pointer) - '0';
            add(row, &height);
            pointer++;
        }
        add(map, row);
    }

    int total_end_positions = 0;
    int total_trailhead_count = 0;
    for (int y = 0; y < map->size; y++) {
        Array* row = (Array*) map->data[y];
        for (int x = 0; x < row->size; x++) {
            int height = *((int*) row->data[x]);
            if (height == 0) {
                HashMap *end_positions = initHashMap(sizeof(Position), map->size * 10);
                total_trailhead_count += count_trailheads(map, x, y, -1, end_positions);
                total_end_positions += mapSize(end_positions);
                freeHashMap(end_positions);
            }
        }
    }

    printf("%d\n", total_end_positions);
    printf("%d\n", total_trailhead_count);
    return 0;
}
