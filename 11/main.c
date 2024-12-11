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

void insertAt(Array *array, int index, void *element) {
    if (array->size == array->capacity) {
        array->data = realloc(array->data, sizeof(void*) * (array->capacity + 10));
        array->capacity += 10;
    }
    for (int i = array->size - 1; i >= index; i--) {
        array->data[i + 1] = array->data[i];
    }
    void *new_element = malloc(array->element_size);
    memcpy(new_element, element, array->element_size);
    array->data[index] = new_element;
    array->size++;
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

typedef struct {
    long stone;
    long count;
} StoneCount;

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

HashMap *copyMap(HashMap *map) {
    HashMap *copy_map = malloc(sizeof(HashMap));

    copy_map->capacity = map->capacity;
    copy_map->data = malloc(sizeof(Array*) * map->capacity);
    copy_map->element_size = map->element_size;

    for (int i = 0; i < map->capacity; i++) {
        copy_map->data[i] = copy((Array*) map->data[i]);
    }

    return copy_map;
}

Array* getMapStones(HashMap *map) {
    Array *stones = initArray(sizeof(long));
    for (int i = 0; i < map->capacity; i++) {
        Array *array = (Array*) map->data[i];
        for (int j = 0; j < array->size; j++) {
            StoneCount* stone_count = (StoneCount*) array->data[j];
            if (stone_count->count > 0) {
                add(stones, &(stone_count->stone));
            }
        }
    }
    return stones;
}

void setStoneCount(HashMap *map, long stone, long count) {
    int hash = stone % map->capacity;

    Array *array = (Array*) map->data[hash];

    for (int i = 0; i < array->size; i++) {
        StoneCount *saved_node = (StoneCount*) array->data[i];
        if (saved_node->stone == stone) {
            saved_node->count = count;
            return;
        }
    }
    StoneCount new_node;
    new_node.stone = stone;
    new_node.count = count;
    add(array, &new_node);
}

long getStoneCount(HashMap *map, long stone) {
    int hash = stone % map->capacity;

    Array *array = (Array*) map->data[hash];

    for (int i = 0; i < array->size; i++) {
        StoneCount *saved_node = (StoneCount*) array->data[i];
        if (saved_node->stone == stone) {
            return saved_node->count;
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

int has_even_digits(long stone) {
    int stone_length = snprintf(NULL, 0,"%lld",stone);
    if (stone_length % 2 == 0) {
        return 1;
    }
    return 0;
}

void transform_stone(HashMap *stones, HashMap *result_stones, long stone) {
    if (stone == 0) {
        long old_stone_count = getStoneCount(stones, 0);
        long result_old_stone_count = getStoneCount(result_stones, 0);
        long new_stone_count = getStoneCount(result_stones, 1);
        setStoneCount(result_stones, 0, result_old_stone_count - old_stone_count);
        setStoneCount(result_stones, 1, new_stone_count + old_stone_count);
        return;
    } else if (has_even_digits(stone) == 1) {
        int stone_length = snprintf(NULL, 0,"%lld",stone);
        char *stone_str = malloc(sizeof(char) * (stone_length + 1));
        sprintf(stone_str, "%lld", stone);
        int first_stone_length = stone_length / 2;
        char *first_stone_str = malloc(sizeof(char) * (first_stone_length + 1));
        int second_stone_length = stone_length / 2;
        char *second_stone_str = malloc(sizeof(char) * (second_stone_length + 1));
        int leading_zero = 1;
        int offset = 0;
        for (int i = 0; i < first_stone_length; i++) {
            if (leading_zero == 1) {
                if (stone_str[i] == 0) {
                    continue;
                } else {
                    leading_zero = 0;
                }
            }
            first_stone_str[offset] = stone_str[i];
            offset++;
        }
        first_stone_str[offset] = '\0';
        leading_zero = 1;
        offset = 0;
        for (int i = 0; i < second_stone_length; i++) {
            if (leading_zero == 1) {
                if (stone_str[i] == 0) {
                    continue;
                } else {
                    leading_zero = 0;
                }
            }
            second_stone_str[offset] = stone_str[first_stone_length + i];
            offset++;
        }
        second_stone_str[offset] = '\0';

        long first_stone, second_stone;
        sscanf(first_stone_str, "%lld", &first_stone);
        sscanf(second_stone_str, "%lld", &second_stone);

        long old_stone_count = getStoneCount(stones, stone);
        long result_old_stone_count = getStoneCount(result_stones, stone);
        setStoneCount(result_stones, stone, result_old_stone_count - old_stone_count);

        long first_stone_count = getStoneCount(result_stones, first_stone);
        setStoneCount(result_stones, first_stone, first_stone_count + old_stone_count);

        long second_stone_count = getStoneCount(result_stones, second_stone);
        setStoneCount(result_stones, second_stone, second_stone_count + old_stone_count);

        free(first_stone_str);
        free(second_stone_str);
        free(stone_str);
        return;
    }
    long new_stone = stone * 2024;
    long old_stone_count = getStoneCount(stones, stone);
    long result_old_stone_count = getStoneCount(result_stones, stone);
    long new_stone_count = getStoneCount(result_stones, new_stone);
    setStoneCount(result_stones, stone, result_old_stone_count - old_stone_count);
    setStoneCount(result_stones, new_stone, new_stone_count + old_stone_count);
}

HashMap *transform_stones(HashMap *stones) {
    HashMap *result_stones = copyMap(stones);
    Array *unique_stones = getMapStones(stones);
    for (int i = 0; i < unique_stones->size; i++) {
        long stone = *((long*) unique_stones->data[i]);
        transform_stone(stones, result_stones, stone);
    }
    freeArray(unique_stones);
    return result_stones;
}

int main(int argc, char** argv) {
    char buffer[BUFFERSIZE];
    char *pointer = buffer;

    HashMap *stones = initHashMap(sizeof(StoneCount), 1000);
    long stone;
    while (fgets(buffer, BUFFERSIZE, stdin)) {
        pointer = buffer;
        if (*pointer == '\n') {
            break;
        }
        while (*pointer != '\n') {
            sscanf(pointer, "%lld", &stone);
            long stone_count = getStoneCount(stones, stone);
            setStoneCount(stones, stone, stone_count+1);
            Array *stone_list = getMapStones(stones);
            while (*pointer != ' ' && *pointer != '\n') {
                pointer++;
            }
            if (*pointer == '\n') {
                break;
            }
            pointer++;
        }
    }

    for (int i = 0; i < 75; i++) {
        HashMap *result_stones = transform_stones(stones);
        stones = result_stones;
    }
    Array *unique_stones = getMapStones(stones);
    long total_stone_count = 0;
    for (int i = 0; i < unique_stones->size; i++) {
        long stone = *((long*) unique_stones->data[i]);
        total_stone_count += getStoneCount(stones, stone);
    }

    printf("%lld\n", total_stone_count);
    return 0;
}
