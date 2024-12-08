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

void detect_antinodes(char current_antenna, int current_antenna_x, int current_antenna_y, Array *map, int **antinodes, int resonant) {
    int dist_x, dist_y;
    int antinode_x, antinode_y;
    for (int y = 0; y < map->size; y++) {
        Array *row = (Array*) map->data[y];
        for (int x = 0; x < row->size; x++) {
            char current_char = *((char*) row->data[x]);
            if (current_char != '.') {
                if (current_antenna == current_char) {
                    dist_x = x - current_antenna_x;
                    dist_y = y - current_antenna_y;

                    if (dist_x == 0 && dist_y == 0) {
                        continue;
                    }

                    if (resonant == 1) {
                        antinode_y = y;
                        antinode_x = x;
                        while (1) {
                            if (antinode_y >= map->size || antinode_y < 0) {
                                break;
                            }
                            if (antinode_x >= row->size || antinode_x < 0) {
                                break;
                            }
                            antinodes[antinode_y][antinode_x] += 1;
                            antinode_y += dist_y;
                            antinode_x += dist_x;
                        }
                        antinode_y = current_antenna_y;
                        antinode_x = current_antenna_x;
                        while (1) {
                            if (antinode_y >= map->size || antinode_y < 0) {
                                break;
                            }
                            if (antinode_x >= row->size || antinode_x < 0) {
                                break;
                            }
                            antinodes[antinode_y][antinode_x] += 1;
                            antinode_y -= dist_y;
                            antinode_x -= dist_x;
                        }
                    } else {
                        antinode_y = y + dist_y;
                        antinode_x = x + dist_x;
                        if (antinode_y < map->size && antinode_y >= 0) {
                            if (antinode_x < row->size && antinode_x >= 0) {
                                antinodes[antinode_y][antinode_x] += 1;
                            }
                        }

                        antinode_y = current_antenna_y - dist_y;
                        antinode_x = current_antenna_x - dist_x;
                        if (antinode_y < map->size && antinode_y >= 0) {
                            if (antinode_x < row->size && antinode_x >= 0) {
                                antinodes[antinode_y][antinode_x] += 1;
                            }
                        }
                    }
                }
            }
        }
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
        Array *row = initArray(sizeof(int));
        while (*pointer != '\n') {
            add(row, pointer);
            pointer++;
        }
        add(map, row);
    }

    int **antinodes = malloc(sizeof(int*) * map->size);
    for (int i = 0; i < map->size; i++) {
        Array *row = (Array*) map->data[i];
        antinodes[i] = malloc(row->size * sizeof(int));
        for (int j = 0; j < row->size; j++) {
            antinodes[i][j] = 0;
        }
    }

    for (int y = 0; y < map->size; y++) {
        Array *row = (Array*) map->data[y];
        for (int x = 0; x < row->size; x++) {
            char current_char = *((char*) row->data[x]);
            if (current_char != '.') {
                detect_antinodes(current_char, x, y, map, antinodes, 0);
            }
        }
    }

    int unique_antinodes_count = 0;
    for (int i = 0; i < map->size; i++) {
        Array *row = (Array*) map->data[i];
        for (int j = 0; j < row->size; j++) {
            if (antinodes[i][j] > 0) {
                unique_antinodes_count++;
            }
        }
    }
    printf("%d\n", unique_antinodes_count);

    for (int y = 0; y < map->size; y++) {
        Array *row = (Array*) map->data[y];
        for (int x = 0; x < row->size; x++) {
            char current_char = *((char*) row->data[x]);
            if (current_char != '.') {
                detect_antinodes(current_char, x, y, map, antinodes, 1);
            }
        }
    }

    unique_antinodes_count = 0;
    for (int i = 0; i < map->size; i++) {
        Array *row = (Array*) map->data[i];
        for (int j = 0; j < row->size; j++) {
            if (antinodes[i][j] > 0) {
                unique_antinodes_count++;
            }
        }
    }

    printf("%d\n", unique_antinodes_count);
    return 0;
}
