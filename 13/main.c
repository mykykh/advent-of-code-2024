#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

int solve_linear(Position *a, Position *b, Position *p, Position *s) {
    double x1 = (double) a->x;
    double y1 = (double) b->x;
    double x2 = (double) a->y;
    double y2 = (double) b->y;
    double b1 = (double) p->x;
    double b2 = (double) p->y;

    if (fabs(round(x1)) < 0.0001) {
        return 1;
    }

    y1 = y1/x1;
    b1 = b1/x1;
    x1 = 1;

    y2 = y2 - x2*y1;
    b2 = b2 - x2*b1;
    x2 = 0;

    if (fabs(round(y2)) < 0.0001) {
        return 1;
    }

    b2 = b2/y2;
    y2 = 1;

    b1 = b1 - y1*b2;
    y1 = 0;

    if (fabs(round(b1) - b1) > 0.01) {
        return 1;
    }
    if (fabs(round(b2) - b2) > 0.01) {
        return 1;
    }

    s->x = round(b1);
    s->y = round(b2);
    return 0;
}

int main(int argc, char** argv) {
    char buffer[BUFFERSIZE];
    char *pointer = buffer;

    Array *a_buttons = initArray(sizeof(Position));
    Array *b_buttons = initArray(sizeof(Position));
    Array *prizes = initArray(sizeof(Position));
    Position temp;
    while (fgets(buffer, BUFFERSIZE, stdin)) {
        pointer = buffer;
        if (*pointer == '\n') {
            fgets(buffer, BUFFERSIZE, stdin);
            pointer = buffer;
            if (*pointer == '\n') {
                break;
            }
        }
        sscanf(buffer, "Button A: X+%lld, Y+%lld", &temp.x, &temp.y);
        add(a_buttons, &temp);
        fgets(buffer, BUFFERSIZE, stdin);
        sscanf(buffer, "Button B: X+%lld, Y+%lld", &temp.x, &temp.y);
        add(b_buttons, &temp);
        fgets(buffer, BUFFERSIZE, stdin);
        sscanf(buffer, "Prize: X=%lld, Y=%lld", &temp.x, &temp.y);
        add(prizes, &temp);
    }

    Position solution;
    long total_price_1 = 0;
    for (int i = 0; i < a_buttons->size; i++) {
        Position *a = (Position*) a_buttons->data[i];
        Position *b = (Position*) b_buttons->data[i];
        Position *p = (Position*) prizes->data[i];
        int result = solve_linear(a, b, p, &solution);
        if (result == 0) {
            total_price_1 += 3 * solution.x + solution.y;
        }
    }

    long total_price_2 = 0;
    for (int i = 0; i < a_buttons->size; i++) {
        Position *a = (Position*) a_buttons->data[i];
        Position *b = (Position*) b_buttons->data[i];
        Position *p = (Position*) prizes->data[i];
        p->x += 10000000000000;
        p->y += 10000000000000;
        int result = solve_linear(a, b, p, &solution);
        if (result == 0) {
            total_price_2 += 3 * solution.x + solution.y;
        }
    }

    printf("%lld\n", total_price_1);
    printf("%lld\n", total_price_2);
    return 0;
}
