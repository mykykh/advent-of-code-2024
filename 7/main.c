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

int get_solution_count(long long int goal, Array* row, long long int buff, int pos, int do_concat) {
    if (pos >= row->size) {
        if (goal == buff) {
            return 1;
        }
        return 0;
    }
    long long int x = *((long long int*) row->data[pos]);
    int result = 0;
    if (buff == 0) {
        result += get_solution_count(goal, row, x, pos+1, do_concat);
    } else {
        result += get_solution_count(goal, row, buff + x, pos+1, do_concat);
        result += get_solution_count(goal, row, buff * x, pos+1, do_concat);
        if (do_concat == 1) {
            int buff_str_length = snprintf(NULL, 0,"%lld",buff);
            char *buff_str = malloc(sizeof(char) * (buff_str_length + 1));
            sprintf(buff_str, "%lld", buff);

            int x_str_length = snprintf(NULL, 0,"%lld",x);
            char* x_str = malloc(sizeof(char) * (x_str_length + 1));
            sprintf(x_str, "%lld", x);

            char *concat_str = malloc(sizeof(char) * (buff_str_length + x_str_length + 1));
            strcpy(concat_str, buff_str);
            strcat(concat_str, x_str);

            long long int concat_x;
            sscanf(concat_str, "%lld", &concat_x);

            result += get_solution_count(goal, row, concat_x, pos+1, do_concat);
        }
    }
    return result;
}

int main(int argc, char** argv) {
    char buffer[BUFFERSIZE];
    char *pointer = buffer;

    Array *b = initArray(sizeof(long long int));
    Array *A = initArray(sizeof(Array));
    long long int temp;
    while (fgets(buffer, BUFFERSIZE, stdin)) {
        pointer = buffer;
        if (*pointer == '\n') {
            break;
        }
        sscanf(pointer, "%lld", &temp);
        add(b, &temp);
        Array *row = initArray(sizeof(long long int));
        while (*pointer != '\n') {
            while (*pointer != ' ' && *pointer != '\n') {
                pointer++;
            }
            if (*pointer == '\n') {
                break;
            }
            if (*pointer == ' ') {
                pointer++;
            }
            sscanf(pointer, "%lld", &temp);
            add(row, &temp);
        }
        add(A, row);
    }

    long long int total_correct = 0;
    long long int total_correct_concat = 0;
    for (int i = 0; i < b->size; i++) {
        long long int goal = *((long long int*) b->data[i]);
        Array *row = (Array*) A->data[i];
        int solution_count = get_solution_count(goal, row, 0, 0, 0);
        int solution_count_concat = get_solution_count(goal, row, 0, 0, 1);
        if (solution_count > 0) {
            total_correct += goal;
        }
        if (solution_count_concat > 0) {
            total_correct_concat += goal;
        }
    }

    printf("%lld\n", total_correct);
    printf("%lld\n", total_correct_concat);

    return 0;
}
