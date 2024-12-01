#include <stdio.h>
#include <stdlib.h>

#define BUFFERSIZE 100

typedef struct {
    int capacity;
    int size;
    int* data;
} Array;

Array *initArray() {
    Array *array = malloc(sizeof(Array));

    array->capacity = 10;
    array->size = 0;
    array->data = malloc(sizeof(int) * array->capacity);

    return array;
}

void add(Array *array, int element) {
    if (array->size == array->capacity) {
        array->data = realloc(array->data, sizeof(int) * (array->capacity + 10));
        array->capacity += 10;
    }
    array->data[array->size] = element;
    array->size += 1;
}

void freeArray(Array *array) {
    free(array->data);
    free(array);
}

void mergeSort(int size, int *data) {
    if (size <= 1) {
        return;
    }

    int leftSize = size/2;
    int rightSize = size - leftSize;
    int *left = malloc(sizeof(int) * leftSize);
    int *right = malloc(sizeof(int) * rightSize);

    for (int i = 0; i < leftSize; i++) {
        left[i] = data[i];
    }
    for (int i = 0; i < rightSize; i++) {
        right[i] = data[leftSize + i];
    }

    mergeSort(leftSize, left);
    mergeSort(rightSize, right);

    int leftPos = 0;
    int rightPos = 0;
    for (int i = 0; i < size; i++) {
        if (leftPos == leftSize) {
            data[i] = right[rightPos];
            rightPos++;
        } else if (rightPos == rightSize) {
            data[i] = left[leftPos];
            leftPos++;
        } else {
            if (left[leftPos] < right[rightPos]) {
                data[i] = left[leftPos];
                leftPos++;
            } else {
                data[i] = right[rightPos];
                rightPos++;
            }
        }
    }

    free(left);
    free(right);
}

void sort(Array *array) {
    mergeSort(array->size, array->data);
}

int main(int argc, char** argv) {
    Array *left = initArray();
    Array *right = initArray();
    char buffer[BUFFERSIZE];

    int maxElement = 0;
    int leftElement, rightElement;
    while (fgets(buffer, BUFFERSIZE, stdin)) {
        if (sscanf(buffer, "%d %d", &leftElement, &rightElement) == 2) {
            add(left, leftElement);
            add(right, rightElement);

            if (leftElement < rightElement) {
                if (maxElement < rightElement) {
                    maxElement = rightElement;
                }
            } else {
                if (maxElement < leftElement) {
                    maxElement = leftElement;
                }
            }
        } else {
            break;
        }
    }

    sort(left);
    sort(right);

    int diff = 0;
    for (int i = 0; i < left->size; i++) {
        diff += abs(left->data[i] - right->data[i]);
    }

    int sim = 0;
    int* rightElementCounts = malloc(sizeof(int) * maxElement);
    for (int i = 0; i < maxElement; i++) {
        rightElementCounts[i] = 0;
    }
    for (int i = 0; i < right->size; i++) {
        rightElementCounts[right->data[i]] += 1;
    }
    for (int i = 0; i < left->size; i++) {
        sim += left->data[i] * rightElementCounts[left->data[i]];
    }

    printf("Diff: %d\n", diff);
    printf("Sim: %d\n", sim);
    free(rightElementCounts);
    freeArray(left);
    freeArray(right);
    return 0;
}
