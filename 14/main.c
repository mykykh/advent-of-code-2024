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

void simulate_robot(Position *p, Position *v, int map_size_x, int map_size_y) {
    p->x += v->x;
    if (p->x >= map_size_x) {
        p->x = p->x % map_size_x;
    } else if (p->x < 0) {
        p->x = map_size_x + p->x;
    }
    p->y += v->y;
    if (p->y >= map_size_y) {
        p->y = p->y % map_size_y;
    } else if (p->y < 0) {
        p->y = map_size_y + p->y;
    }
}

double robots_std(Array *robot_ps) {
    double mean_x = 0;
    double mean_y = 0;
    for (int i = 0; i < robot_ps->size; i++) {
        Position *p = (Position*) robot_ps->data[i];
        mean_x += p->x;
        mean_y += p->y;
    }
    mean_x /= robot_ps->size;
    mean_y /= robot_ps->size;

    double std_x = 0;
    double std_y = 0;
    for (int i = 0; i < robot_ps->size; i++) {
        Position *p = (Position*) robot_ps->data[i];
        std_x += (p->x - mean_x) * (p->x - mean_x);
        std_y += (p->y - mean_y) * (p->y - mean_y);
    }
    std_x = sqrt(std_x / robot_ps->size);
    std_y = sqrt(std_y / robot_ps->size);
    return (std_x + std_y) / 2;
}

void print_robots(Array *robot_ps, int map_size_x, int map_size_y) {
    for (int y = 0; y < map_size_y; y++) {
        for (int x = 0; x < map_size_x; x++) {
            int r = 0;
            for (int i = 0; i < robot_ps->size; i++) {
                Position *p = (Position*) robot_ps->data[i];
                if (p->x == x && p->y == y) {
                    r = 1;
                    printf("r");
                    break;
                }
            }
            if (r == 0) {
                printf(".");
            }
        }
        printf("\n");
    }
}

void simulate_robots(int steps, Array *robot_ps, Array *robot_vs, int map_size_x, int map_size_y) {
    for (int step = 0; step < steps; step++) {
        for (int i = 0; i < robot_ps->size; i++) {
            Position *p = (Position*) robot_ps->data[i];
            Position *v = (Position*) robot_vs->data[i];
            simulate_robot(p, v, map_size_x, map_size_y);
        }
        double std = robots_std(robot_ps);
        if (std < 23) {
            printf("%d %f\n", step+1, std);
            print_robots(robot_ps, map_size_x, map_size_y);
            break;
        }
    }
}

int main(int argc, char** argv) {
    char buffer[BUFFERSIZE];
    char *pointer = buffer;

    Array *robot_ps = initArray(sizeof(Position));
    Array *robot_vs = initArray(sizeof(Position));
    Position temp_p, temp_v;
    while (fgets(buffer, BUFFERSIZE, stdin)) {
        pointer = buffer;
        if (*pointer == '\n') {
            break;
        }
        sscanf(buffer, "p=%lld,%lld v=%lld,%lld", &temp_p.x, &temp_p.y, &temp_v.x, &temp_v.y);
        add(robot_ps, &temp_p);
        add(robot_vs, &temp_v);
    }

    int map_size_x = 101;
    int map_size_y = 103;
    simulate_robots(100000, robot_ps, robot_vs, map_size_x, map_size_y);

    long r_count_tl = 0, r_count_tr = 0, r_count_bl = 0, r_count_br = 0;
    for (int i = 0; i < robot_ps->size; i++) {
        Position *p = (Position*) robot_ps->data[i];
        if (p->x < map_size_x/2) {
            if (p->y < map_size_y/2) {
                r_count_tl++;
            } else if (p->y >= (map_size_y/2) + 1) {
                r_count_bl++;
            }
        } else if (p->x >= (map_size_x/2) + 1) {
            if (p->y < map_size_y/2) {
                r_count_tr++;
            } else if (p->y >= (map_size_y/2) + 1) {
                r_count_br++;
            }
        }
    }

    printf("%lld\n", r_count_tl * r_count_tr * r_count_bl * r_count_br);
    return 0;
}
