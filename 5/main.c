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

int contains(Array *array, int element) {
    for (int i = 0; i < array->size; i++) {
        if (element == *((int*) array->data[i])) {
            return 1;
        }
    }
    return 0;
}

int main(int argc, char** argv) {
    char buffer[BUFFERSIZE];
    char *pointer = buffer;

    Array *rules = initArray(sizeof(int));
    int number1, number2;
    while (fgets(buffer, BUFFERSIZE, stdin)) {
        if (buffer[0] == '\n') {
            break;
        }
        int numberCount = sscanf(buffer, "%d|%d", &number1, &number2);
        if (numberCount != 2) {
            break;
        }
        add(rules, &number1);
        add(rules, &number2);
    }

    Array *updates = initArray(sizeof(Array));
    int number;
    while (fgets(buffer, BUFFERSIZE, stdin)) {
        pointer = buffer;
        if (*pointer == '\n') {
            break;
        }
        Array *update = initArray(sizeof(int));
        while (*pointer != '\n') {
            sscanf(pointer, "%d", &number);
            add(update, &number);
            while (*pointer != ',' && *pointer != '\n') {
                pointer++;
            }
            if (*pointer == ',') {
                pointer++;
            }
        }
        add(updates, update);
    }

    int maxRule = 0;
    for (int i = 0; i < rules->size; i++) {
        int rule = *((int*) rules->data[i]);
        if (maxRule < rule) {
            maxRule = rule;
        }
    }

    Array **y_x_rules = malloc((maxRule+1) * sizeof(Array*));
    for (int i = 0; i < maxRule+1; i++) {
        y_x_rules[i] = initArray(sizeof(int));
    }

    for (int i = 1; i < rules->size; i += 2) {
        int y = *((int*) rules->data[i]);
        int x = *((int*) rules->data[i-1]);
        Array *x_rules = ((Array*) y_x_rules[y]);
        add(x_rules, &x);
    }

    int sum = 0;
    for (int i = 0; i < updates->size; i++) {
        Array *update = ((Array*) updates->data[i]);
        int result = 0;
        for (int j = 0; j < update->size; j++) {
            number1 = *((int*) update->data[j]);
            for (int k = j+1; k < update->size; k++) {
                number2 = *((int*) update->data[k]);
                result = contains(y_x_rules[number1], number2);
                if (result == 1) {
                    break;
                }
            }
            if (result == 1) {
                break;
            }
        }
        if (result == 1) {
            continue;
        }
        int middle_id = update->size / 2;
        number = *((int*) update->data[middle_id]);
        sum += number;
    }
    printf("Correct sum: %d\n", sum);

    int incorrect_sum = 0;
    for (int i = 0; i < updates->size; i++) {
        Array *update = ((Array*) updates->data[i]);
        int result = 0;
        int correct = 1;
        int add_sum = 0;
        for (int j = 0; j < update->size; j++) {
            do {
                correct = 1;
                number1 = *((int*) update->data[j]);
                for (int k = j+1; k < update->size; k++) {
                    number2 = *((int*) update->data[k]);
                    result = contains(y_x_rules[number1], number2);
                    if (result == 1) {
                        set(update, j, &number2);
                        set(update, k, &number1);
                        correct = 0;
                        add_sum = 1;
                        break;
                    }
                }
            } while (correct != 1);
        }

        if (add_sum == 0) {
            continue;
        }
        int middle_id = update->size / 2;
        number = *((int*) update->data[middle_id]);
        incorrect_sum += number;
    }
    printf("Incorrect sum: %d\n", incorrect_sum);
    freeArray(rules);
    freeArray(updates);
    return 0;
}
