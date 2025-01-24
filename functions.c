#include "Utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

/*
	Character arrays for the two encoding schemes used in the EAN-8 standard
*/
const char *ean8_left[10] = {
    "0001101", "0011001", "0010011", "0111101", "0100011",
    "0110001", "0101111", "0111011", "0110111", "0001011"
};

const char *ean8_right[10] = {
    "1110010", "1100110", "1101100", "1000010", "1011100",
    "1001110", "1010000", "1000100", "1001000", "1110100"
};

/*
    Function to performs identifier validation
*/
bool check_identifier(const char *identifier) {
    int sum = 0;

    for (int i = 0; i < 7; i++) {  // Calculando soma dos primeiros 7 dígitos
        if (identifier[i] < '0' || identifier[i] > '9') {
            fprintf(stderr, "Error! The identifier contains non-numeric characters. Only numbers are allowed.\n");
            return false;
        }

        int digit = identifier[i] - '0';

        sum += (i % 2 == 0) ? digit * 3 : digit;
    }

    int check_digit = (10 - (sum % 10)) % 10;

    if (check_digit != (identifier[7] - '0')) {
        fprintf(stderr, "Error! Invalid identifier.\n");
        return false;
    }

    return true;
}


/*
    Auxiliary function to write binary patterns to PBM file
*/
void write_pattern(FILE *file, const char *pattern, int bar_width) {
    for (int i = 0; pattern[i] != '\0'; i++) {
        for (int w = 0; w < bar_width; w++) {
            fprintf(file, "%d ", pattern[i] - '0');
        }
    }
}

/*
    Function to generate the barcode from an EAN-8 identifier
*/
bool generate_code(const BarCode *config) {
    if (!check_identifier(config->identifier)) {
        return false;
    }

    FILE *file = fopen(config->filename, "w");
    if (!file) {
        fprintf(stderr, "Error! Could not create file %s.\n", config->filename);
        return false;
    }

    const char *start_marker = "101";
    const char *center_marker = "01010";
    const char *end_marker = "101";

    int code_width = config->spacing * 2 + (3 + 28 + 5 + 28 + 3) * config->bar_width;
    int total_height = config->height + 2 * config->spacing;

    fprintf(file, "P1\n%d %d\n", code_width, total_height);

    // top margin
    for (int y = 0; y < config->spacing; y++) {
        for (int x = 0; x < code_width; x++) {
            fprintf(file, "0 ");
        }
        fprintf(file, "\n");
    }

    for (int y = 0; y < config->height; y++) {
        for (int x = 0; x < config->spacing; x++) {
            fprintf(file, "0 ");
        }

        write_pattern(file, start_marker, config->bar_width);

        for (int i = 0; i < 4; i++) {
            const char *pattern = ean8_left[config->identifier[i] - '0'];
            write_pattern(file, pattern, config->bar_width);
        }

        // center
        write_pattern(file, center_marker, config->bar_width);

        //
        for (int i = 4; i < 8; i++) {
            const char *pattern = ean8_right[config->identifier[i] - '0'];
            write_pattern(file, pattern, config->bar_width);
        }


        write_pattern(file, end_marker, config->bar_width);

        for (int x = 0; x < config->spacing; x++) {
            fprintf(file, "0 ");
        }

        fprintf(file, "\n");
    }

    // down margin
    for (int y = 0; y < config->spacing; y++) {
        for (int x = 0; x < code_width; x++) {
            fprintf(file, "0 ");
        }
        fprintf(file, "\n");
    }

    fclose(file);
    return true;
}


/*
    Function to check PBM file header
*/
bool validate_pbm_header(FILE *file, int *bar_width, int *height) {
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = '\0';

        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        if (strcmp(line, "P1") == 0) {
            break;
        } else {
            fprintf(stderr, "Error! The file does not follow the expected PBM format (P1).\n");
            return false;
        }
    }

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\r\n")] = '\0';

        if (line[0] == '\0' || line[0] == '#') {
            continue;
        }

        if (sscanf(line, "%d %d", bar_width, height) == 2) {
            return true;
        } else {
            fprintf(stderr, "Error! Invalid dimensions in file header.\n");
            return false;
        }
    }

    fprintf(stderr, "Error! The PBM file header is incomplete or poorly formatted.\n");
    return false;
}


/*
    Function to check the start, center and end markers of the barcode
*/

bool validate_markers(int **matriz, int bar_width, int height, int N, char *binary_seq) {
    int spacing = (bar_width - (N * 67)) / 2;
    int pos = 0;

    for (int i = spacing; i < bar_width - spacing; i += N) {
        int sum = 0;
        for (int j = i; j < i + N; j++) {
            sum += matriz[height / 2][j];
        }
        binary_seq[pos++] = (sum > (N / 2)) ? '1' : '0';
    }
    binary_seq[pos] = '\0';

    const int start_pos = 0;
    const int center_pos = 3 + 28;
    const int end_pos = center_pos + 5 + 28;
    struct {
        const char *marker;
        int position;
        const char *error_message;
    } markers[] = {
        {"101", start_pos, "Error! Invalid start marker.\n"},
        {"01010", center_pos, "Error! Invalid center marker.\n"},
        {"101", end_pos, "EError! Invalid end marker.\n"}
    };

    for (int i = 0; i < 3; i++) {
        if (strncmp(&binary_seq[markers[i].position], markers[i].marker, strlen(markers[i].marker)) != 0) {
            fprintf(stderr, "%s", markers[i].error_message);
            return false;
        }
    }

    return true;
}

/*
    Function to extract the decoded identifier from the barcode
*/
bool decode_identifier(const char *binary_seq, char *identifier) {
    int pos = 0;
    for (int i = 3; i < 31; i += 7) {
        bool found = false;
        for (int j = 0; j < 10; j++) {
            if (strncmp(&binary_seq[i],ean8_left[j], 7) == 0) {
                identifier[pos++] = '0' + j;
                found = true;
                break;
            }
        }
        if (!found) {
            printf("Error! Invalid L-Code Sequence.\n");
            return false;
        }
    }

    for (int i = 36; i < 64; i += 7) {
        bool found = false;
        for (int j = 0; j < 10; j++) {
            if (strncmp(&binary_seq[i], ean8_right[j], 7) == 0) {
                identifier[pos++] = '0' + j;
                found = true;
                break;
            }
        }
        if (!found) {
            printf("Error! Invalid R-Code Sequence.\n");
            return false;
        }
    }

    identifier[pos] = '\0';
    return true;
}