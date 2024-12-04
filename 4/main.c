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

int detectWord(int word_size, char* word, int current_c, Array* lines, int current_i, int current_j, int i_dir, int j_dir) {
    if (current_c > word_size - 1) {
        return 0;
    }
    char target_char = word[current_c];

    if (current_i < 0) {
        return 0;
    }
    if (current_i >= lines->size) {
        return 0;
    }
    Array* temp_line = (Array*) lines->data[current_i];
    if (current_j < 0) {
        return 0;
    }
    if (current_j >= temp_line->size) {
        return 0;
    }
    char temp_char = *((char*) temp_line->data[current_j]);

    if (temp_char != target_char) {
        return 0;
    }

    if (current_c == word_size - 1) {
        return 1;
    }

    int result = 0;
    result += detectWord(word_size, word, current_c+1, lines, current_i+i_dir, current_j+j_dir, i_dir, j_dir);

    return result;
}

int detectXWord(int word_size, char* word, int current_c, Array* lines, int current_i, int current_j) {
    int result = 0;
    int first_word = 0;
    int second_word = 0;
    first_word = detectWord(word_size, word, current_c, lines, current_i, current_j, 1, -1);
    if (first_word == 1) {
        second_word = detectWord(word_size, word, current_c, lines, current_i + 2, current_j, -1, -1);
        if (second_word == 1) {
            result++;
        }
    }
    first_word = detectWord(word_size, word, current_c, lines, current_i, current_j, 1, 1);
    if (first_word == 1) {
        second_word = detectWord(word_size, word, current_c, lines, current_i + 2, current_j, -1, 1);
        if (second_word == 1) {
            result++;
        }
    }
    first_word = detectWord(word_size, word, current_c, lines, current_i, current_j, -1, 1);
    if (first_word == 1) {
        second_word = detectWord(word_size, word, current_c, lines, current_i, current_j + 2, -1, -1);
        if (second_word == 1) {
            result++;
        }
    }
    first_word = detectWord(word_size, word, current_c, lines, current_i, current_j, 1, 1);
    if (first_word == 1) {
        second_word = detectWord(word_size, word, current_c, lines, current_i, current_j + 2, 1, -1);
        if (second_word == 1) {
            result++;
        }
    }

    return result;
}

int main(int argc, char** argv) {
    char buffer[BUFFERSIZE];
    char *pointer = buffer;

    Array *lines = initArray(sizeof(Array));
    while (fgets(buffer, BUFFERSIZE, stdin)) {
        pointer = buffer;
        if (*pointer == '\n') {
            break;
        }
        Array *line = initArray(sizeof(char));
        while (*pointer != '\n') {
            add(line, pointer);
            pointer++;
        }
        add(lines, line);
    }

    char *target_word_1 = "XMAS";
    char *target_word_2 = "MAS";

    int count = 0;
    int Xcount = 0;
    for (int i = 0; i < lines->size; i++) {
        Array *line = (Array*) lines->data[i];
        for (int j = 0; j < line->size; j++) {
            count += detectWord(4, target_word_1, 0, lines, i, j, -1, -1);
            count += detectWord(4, target_word_1, 0, lines, i, j, -1, 0);
            count += detectWord(4, target_word_1, 0, lines, i, j, -1, 1);
            count += detectWord(4, target_word_1, 0, lines, i, j, 0, -1);
            count += detectWord(4, target_word_1, 0, lines, i, j, 0, 1);
            count += detectWord(4, target_word_1, 0, lines, i, j, 1, -1);
            count += detectWord(4, target_word_1, 0, lines, i, j, 1, 0);
            count += detectWord(4, target_word_1, 0, lines, i, j, 1, 1);

            Xcount += detectXWord(3, target_word_2, 0, lines, i, j);
        }
    }

    printf("XMAS Count: %d\n", count);
    printf("X-MAS Count: %d\n", Xcount);
    freeArray(lines);
    return 0;
}
