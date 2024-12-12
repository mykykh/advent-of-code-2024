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

void get_region(Array *map, int x, int y, char region_type, HashMap *region) {
    if (y < 0 || y >= map->size) {
        return;
    }

    Array *row = (Array*) map->data[y];
    if (x < 0 || x >= map->size) {
        return;
    }

    char plot_type = *((char*) row->data[x]);

    if (plot_type != region_type) {
        return;
    }

    Position position;
    position.x = x;
    position.y = y;

    if (containsPosition(region, &position) == 1) {
        return;
    }

    addPosition(region, &position);

    get_region(map, x, y-1, region_type, region);
    get_region(map, x, y+1, region_type, region);
    get_region(map, x-1, y, region_type, region);
    get_region(map, x+1, y, region_type, region);
}

int in_any_region(Array *regions, int x, int y) {
    Position position;
    position.x = x;
    position.y = y;

    for (int i = 0; i < regions->size; i++) {
        HashMap *region = (HashMap*) regions->data[i];
        if (containsPosition(region, &position) == 1) {
            return 1;
        }
    }

    return 0;
}

Array *get_regions(Array *map) {
    Array *regions = initArray(sizeof(HashMap));
    for (int y = 0; y < map->size; y++) {
        Array *row = (Array*) map->data[y];
        for (int x = 0; x < row->size; x++) {
            if (in_any_region(regions, x, y) == 1) {
                continue;
            }

            char plot_type = *((char*) row->data[x]);

            HashMap *region = initHashMap(sizeof(Position), map->size);
            get_region(map, x, y, plot_type, region);

            add(regions, region);
            free(region);
        }
    }

    return regions;
}

int get_perimeter(Array *map, HashMap *region) {
    Array *region_positions = getPositions(region);
    int perimeter = 0;
    Position temp;
    for (int i = 0; i < region_positions->size; i++) {
        Position *position = (Position*) region_positions->data[i];
        temp.x = position->x;
        temp.y = position->y - 1;
        if (temp.y >= 0 && temp.y < map->size) {
            Array *row = (Array*) map->data[temp.y];
            if (temp.x >= 0 && temp.x < row->size) {
                if (containsPosition(region, &temp) == 0) {
                    perimeter++;
                }
            } else {
                perimeter++;
            }
        } else {
            perimeter++;
        }

        temp.x = position->x;
        temp.y = position->y + 1;
        if (temp.y >= 0 && temp.y < map->size) {
            Array *row = (Array*) map->data[temp.y];
            if (temp.x >= 0 && temp.x < row->size) {
                if (containsPosition(region, &temp) == 0) {
                    perimeter++;
                }
            } else {
                perimeter++;
            }
        } else {
            perimeter++;
        }

        temp.x = position->x - 1;
        temp.y = position->y;
        if (temp.y >= 0 && temp.y < map->size) {
            Array *row = (Array*) map->data[temp.y];
            if (temp.x >= 0 && temp.x < row->size) {
                if (containsPosition(region, &temp) == 0) {
                    perimeter++;
                }
            } else {
                perimeter++;
            }
        } else {
            perimeter++;
        }

        temp.x = position->x + 1;
        temp.y = position->y;
        if (temp.y >= 0 && temp.y < map->size) {
            Array *row = (Array*) map->data[temp.y];
            if (temp.x >= 0 && temp.x < row->size) {
                if (containsPosition(region, &temp) == 0) {
                    perimeter++;
                }
            } else {
                perimeter++;
            }
        } else {
            perimeter++;
        }
    }
    freeArray(region_positions);
    return perimeter;
}

int detect_corner(Array *map, HashMap *region, int x1, int y1, int x2, int y2) {
    Position temp;
    temp.x = x1;
    temp.y = y1;
    if (temp.y >= 0 && temp.y < map->size) {
        Array *row = (Array*) map->data[temp.y];
        if (temp.x >= 0 && temp.x < row->size) {
            if (containsPosition(region, &temp) == 1) {
                return 0;
            }
        }
    }
    int corner_count = 0;
    temp.x = x2;
    temp.y = y2;
    if (temp.y >= 0 && temp.y < map->size) {
        Array *row = (Array*) map->data[temp.y];
        if (temp.x >= 0 && temp.x < row->size) {
            if (containsPosition(region, &temp) == 0) {
                corner_count++;
            }
        } else {
            corner_count++;
        }
    } else {
        corner_count++;
    }

    return corner_count;
}

int detect_inner_corner(Array *map, HashMap *region, int x1, int y1, int x2, int y2, int x3, int y3) {
    Position temp;
    temp.x = x1;
    temp.y = y1;
    if (temp.y >= 0 && temp.y < map->size) {
        Array *row = (Array*) map->data[temp.y];
        if (temp.x >= 0 && temp.x < row->size) {
            if (containsPosition(region, &temp) == 1) {
                return 0;
            }
        }
    }
    temp.x = x2;
    temp.y = y2;
    if (temp.y >= 0 && temp.y < map->size) {
        Array *row = (Array*) map->data[temp.y];
        if (temp.x >= 0 && temp.x < row->size) {
            if (containsPosition(region, &temp) == 0) {
                return 0;
            }
        } else {
            return 0;
        }
    } else {
        return 0;
    }

    temp.x = x3;
    temp.y = y3;
    if (temp.y >= 0 && temp.y < map->size) {
        Array *row = (Array*) map->data[temp.y];
        if (temp.x >= 0 && temp.x < row->size) {
            if (containsPosition(region, &temp) == 0) {
                return 0;
            }
        } else {
            return 0;
        }
    } else {
        return 0;
    }

    return 1;
}

int get_corner_count(Array *map, HashMap *region) {
    Array *region_positions = getPositions(region);
    int corner_count = 0;
    for (int i = 0; i < region_positions->size; i++) {
        Position *position = (Position*) region_positions->data[i];
        int x = position->x;
        int y = position->y;
        int top_left = detect_corner(map, region, x, y-1, x-1, y);
        int top_right = detect_corner(map, region, x, y-1, x+1, y);
        int bottom_left = detect_corner(map, region, x, y+1, x-1, y);
        int bottom_right = detect_corner(map, region, x, y+1, x+1, y);
        int top_left_inner = detect_inner_corner(map, region, x-1, y-1, x-1, y, x, y-1);
        int top_right_inner = detect_inner_corner(map, region, x+1, y-1, x+1, y, x, y-1);
        int bottom_left_inner = detect_inner_corner(map, region, x-1, y+1, x-1, y, x, y+1);
        int bottom_right_inner = detect_inner_corner(map, region, x+1, y+1, x+1, y, x, y+1);
        corner_count += top_left + top_right + bottom_left + bottom_right;
        corner_count += top_left_inner + top_right_inner + bottom_left_inner + bottom_right_inner;
    }
    freeArray(region_positions);
    return corner_count;
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

    Array *regions = get_regions(map);

    int total_sum_1 = 0;
    int total_sum_2 = 0;
    for (int i = 0; i < regions->size; i++) {
        HashMap *region = (HashMap*) regions->data[i];
        int area = mapSize(region);
        int perimeter = get_perimeter(map, region);
        int corner_count = get_corner_count(map, region);
        total_sum_1 += area * perimeter;
        total_sum_2 += area * corner_count;
    }

    printf("%d\n", total_sum_1);
    printf("%d\n", total_sum_2);
    return 0;
}
