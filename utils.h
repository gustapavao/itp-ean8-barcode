#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdbool.h>

/*
    Struct to encapsulate barcode generation parameters
*/
typedef struct {
    const char *identifier;
    const char *filename;
    int spacing;
    int bar_width;
    int height;
} BarCode;

bool check_identifier(const char *identifier);

bool generate_code(const BarCode *config);

bool validate_pbm_header(FILE *file, int *bar_width, int *height);

bool validate_markers(int **matriz, int bar_width, int height, int N, char *binary_seq);

bool decode_identifier(const char *binary_seq, char *identifier);

#endif