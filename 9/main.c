#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE 65000

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

void part_1_compress(Array *input) {
    int compressed = 0;
    int number, temp;
    for (int i = input->size - 1; i >= 0; i--) {
        number = *((int*) input->data[i]);
        for (int j = 0; j <= i; j++) {
            if (i == j) {
                compressed = 1;
                break;
            }
            temp = *((int*) input->data[j]);
            if (temp == -1) {
                set(input, j, &number);
                set(input, i, &temp);
                break;
            }
        }
        if (compressed == 1) {
            break;
        }
    }
}

void part_2_compress(Array *input) {
    int number, temp;
    int length = 0;
    int space_length = 0;
    for (int i = input->size - 1; i >= 0; i--) {
        number = *((int*) input->data[i]);
        if (number == -1) {
            continue;
        }
        length = 1;
        while (1) {
            i--;
            if (i < 0) {
                i++;
                break;
            }
            temp = *((int*) input->data[i]);
            if (temp != number) {
                i++;
                break;
            }
            length += 1;
        }
        for (int j = 0; j < i; j++) {
            temp = *((int*) input->data[j]);
            if (temp == -1) {
                space_length = 1;
                while (1) {
                    j++;
                    if (j >= input->size) {
                        j--;
                        break;
                    }
                    temp = *((int*) input->data[j]);
                    if (temp != -1) {
                        break;
                    }
                    space_length += 1;
                }
                if (space_length >= length) {
                    temp = -1;
                    for (int k = 0; k < length; k++) {
                        set(input, j-space_length+k, &number);
                        set(input, i+k, &temp);
                    }
                    break;
                }
            }
        }
    }
}

long get_sum(Array *input) {
    long result = 0;
    for (int i = 0; i < input->size; i++) {
        int number = *((int*) input->data[i]);
        if (number == -1) {
            continue;
        }
        result += i * number;
    }
    return result;
}

int main(int argc, char** argv) {
    FILE* input_file = fopen("data.in", "r");

    Array *input = initArray(sizeof(int));
    char current_char;
    int number, id = 0, temp;
    while (1) {
        current_char = fgetc(input_file);
        number = current_char - '0';
        for (int i = 0; i < number; i++) {
            add(input, &id);
        }

        current_char = fgetc(input_file);
        if (current_char == '\n') {
            break;
        }

        number = current_char - '0';
        for (int i = 0; i < number; i++) {
            temp = -1;
            add(input, &temp);
        }
        id++;
    }

    fclose(input_file);

    Array *part_1_input = copy(input);
    part_1_compress(part_1_input);
    printf("%lld\n", get_sum(part_1_input));

    Array *part_2_input = copy(input);
    part_2_compress(part_2_input);
    printf("%lld\n", get_sum(part_2_input));

    return 0;
}
