#include "Utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int **allocate_array(int height, int width) {
    int **matriz = (int **)malloc(height * sizeof(int *));
    if (!matriz) {
        fprintf(stderr, "Memory allocation error for matrix.\n");
        return NULL;
    }

    for (int i = 0; i < height; i++) {
        matriz[i] = (int *)malloc(width * sizeof(int));
        if (!matriz[i]) {
            fprintf(stderr, "Memory allocation error for matrix row.\n");
            while (i > 0) free(matriz[--i]);
            free(matriz);
            return NULL;
        }
    }

    return matriz;
}

void release_matrix(int **matriz, int height) {
    for (int i = 0; i < height; i++) {
        free(matriz[i]);
    }
    free(matriz);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Error! File name must be provided.\n");
        return 1;
    }

    const char *filename = argv[1];
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error! Could not open file: %s\n", filename);
        return 1;
    }

    int width, height;
    if (!validate_pbm_header(file, &width, &height)) {
        fclose(file);
        return 1;
    }

    int **matriz = allocate_array(height, width);
    if (!matriz) {
        fclose(file);
        return 1;
    }

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (fscanf(file, "%d", &matriz[i][j]) != 1) {
                fprintf(stderr, "Error reading PBM file.\n");
                fclose(file);
                release_matrix(matriz, height);
                return 1;
            }
        }
    }
    fclose(file);

    int N = width / 67;
    char binary_sequence[68];
    if (validate_markers(matriz, width, height, N, binary_sequence)) {
        printf("\nBinary sequence: %s\n", binary_sequence);

        char identifier[9];
        if (decode_identifier(binary_sequence, identifier)) {
            printf("Decoded identifier: %s\n", identifier);
        } else {
            printf("Error decoding digits.\n");
        }
    } else {
        printf("Error: Invalid barcode.\n");
    }

    release_matrix(matriz, height);

    return 0;
}
