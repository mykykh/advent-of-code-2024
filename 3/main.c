#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main(int argc, char** argv) {
    char buffer[BUFFERSIZE];
    char *pointer = buffer;

    Array *numbers = initArray(sizeof(int));
    int number1, number2;
    int apply = 1;
    while (fgets(buffer, BUFFERSIZE, stdin)) {
        pointer = buffer;
        if (*pointer == '\n') {
            break;
        }
        while (*pointer != '\n') {
            if (*pointer == 'd') {
                if (strncmp(pointer, "don't()", 7) == 0) {
                    apply = 0;
                } else if (strncmp(pointer, "do()", 4) == 0) {
                    apply = 1;
                }
            } else {
                char lastChar;
                int detectedCount = sscanf(pointer, "mul(%d,%d%c", &number1, &number2, &lastChar);

                if (detectedCount == 3) {
                    if (lastChar == ')') {
                        if (apply == 1) {
                            add(numbers, &number1);
                            add(numbers, &number2);
                        }
                    }
                }
            }

            pointer++;
            while (*pointer != '\n' && *pointer != 'm' && *pointer != 'd') {
                pointer++;
            }
        }
    }

    int sum = 0;
    for (int i = 1; i < numbers->size; i += 2) {
        sum += *((int*) numbers->data[i-1]) * *((int*) numbers->data[i]);
    }

    printf("Sum: %d\n", sum);
    freeArray(numbers);
    return 0;
}
