#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE 100

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

int check(int left, int right, int ord) {
    if (left < right) {
        if (ord == -1) {
            return 0;
        }
    } else if (left > right) {
        if (ord == 1) {
            return 0;
        }
    } else {
        return 0;
    }

    if (abs(left - right) > 3) {
        return 0;
    }

    return 1;
}

int reportCheck(Array *report, int ord) {
    if (report->size < 2) {
        return 1;
    }
    for (int j = 1; j < report->size; j++) {
        int left = *((int*) report->data[j - 1]);
        int right = *((int*) report->data[j]);

        int result = check(left, right, ord);
        if (result == 0) {
            return 0;
        }
    }
    return 1;
}

int softCheck(Array *report, int ord, int tolerance) {
    int result = reportCheck(report, ord);
    if (result == 0) {
        if (tolerance > 0) {
            tolerance--;
            for (int i = 0; i < report->size; i++) {
                Array *temp = copy(report);
                delete(temp, i);
                result = softCheck(temp, ord, tolerance);
                if (result == 1) {
                    free(temp->data);
                    free(temp);
                    return 1;
                }
                free(temp->data);
                free(temp);
            }
            return 0;
        } else {
            return 0;
        }
    }
    return 1;
}

int reportsCheck(Array *reports, int tolerance) {
    Array *report;
    int safe_count = 0;
    for (int i = 0; i < reports->size; i++) {
        report = (Array*) reports->data[i];
        if (report->size < 2) {
            continue;
        }
        int result = softCheck(report, -1, tolerance);
        if (result > 0) {
            safe_count++;
            continue;
        }

        result = softCheck(report, 1, tolerance);
        if (result > 0) {
            safe_count++;
            continue;
        }
    }

    return safe_count;
}

int main(int argc, char** argv) {
    char buffer[BUFFERSIZE];
    char *pointer = buffer;

    Array *reports = initArray(sizeof(Array));
    Array *report;
    int element;
    while (fgets(buffer, BUFFERSIZE, stdin)) {
        pointer = buffer;
        if (sscanf(pointer, "%d", &element) == 1) {
            report = initArray(sizeof(int));
            add(report, &element);

            while (*pointer != '\0' && (*pointer >= '0' && *pointer <= '9')) {
                pointer++;
            }
            while (*pointer != '\0' && (*pointer < '0' || *pointer > '9')) {
                pointer++;
            }
            while (sscanf(pointer, "%d", &element) == 1) {
                add(report, &element);
                while (*pointer != '\0' && (*pointer >= '0' && *pointer <= '9')) {
                    pointer++;
                }
                while (*pointer != '\0' && (*pointer < '0' || *pointer > '9')) {
                    pointer++;
                }
            }

            add(reports, report);
        } else {
            break;
        }
    }

    printf("Hard check: %d\n", reportsCheck(reports, 0));
    printf("Soft check: %d\n", reportsCheck(reports, 1));
    freeArray(reports);
    return 0;
}
