#include "Utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define SPACING_DEFAULT 4
#define BAR_WIDTH_DEFAULT 3
#define HEIGHT_DEFAULT 50

/*
    Function to check and validate input parameters
*/
bool validate_args(int argc, char *argv[], BarCode *config) {
    if (argc < 3) {
        fprintf(stderr, "You are missing arguments\n", argv[0]);
        return false;
    }

    config->identifier = argv[1];
    config->filename = argv[2];
    config->spacing = (argc > 3) ? atoi(argv[3]) : SPACING_DEFAULT;
    config->bar_width = (argc > 4) ? atoi(argv[4]) : BAR_WIDTH_DEFAULT;
    config->height = (argc > 5) ? atoi(argv[5]) : HEIGHT_DEFAULT;

    const char *extension = strrchr(config->filename, '.');
    if (!extension || strcasecmp(extension, ".pbm") != 0) {
        fprintf(stderr, "Error! The output file must have the extension .pbm\n");
        return false;
    }

    if (config->spacing <= 0 || config->bar_width <= 0 || config->height <= 0) {
        fprintf(stderr, "Error! Spacing, width, and height must be positive values.\n");
        return false;
    }

    return true;
}

/*
    Function to check if the output file already exists
*/
bool check_file_exists(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file) {
        fclose(file);
        return true;
    }
    return false;
}

/*
    Function to ask the user if he wants to overwrite the file
*/
bool ask_overwrite_permission(const char *filename) {
    char answer[3];
    printf("The file '%s' already exists. Do you want to overwrite it? (Y/N): ", filename);
    if (fgets(answer, sizeof(answer), stdin)) {
        if (answer[0] == 'Y' || answer[0] == 'y') {
            return true;
        }
    }
    return false;
}

int main(int argc, char *argv[]) {
    BarCode config;

    if (!validate_args(argc, argv, &config)) {
        return EXIT_FAILURE;
    }

    if (check_file_exists(config.filename)) {
        if (!ask_overwrite_permission(config.filename)) {
            fprintf(stderr, "Error! The resulting file already exists and will not be overwritten.\n");
            return EXIT_FAILURE;
        }
    }

    if (!generate_code(&config)) {
        return EXIT_FAILURE;
    }

    printf("Barcode image created successfully! You can view it in the file: %s\n", config.filename);
    return EXIT_SUCCESS;
}
