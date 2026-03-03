#include "lib.h"

char *get_comm_data(char *file_name)
{
    FILE *file = fopen(file_name, "r");
    if (!file) {
        perror("fopen");
        return NULL;
    }

    char buffer[256];
    if (!fgets(buffer, sizeof(buffer), file)) {
        perror("fgets");
        fclose(file);
        return NULL;
    }

    fclose(file);

    char *newline = strchr(buffer, '\n');
    if (newline) *newline = '\0';

    char *result = malloc(strlen(buffer) + 1);
    if (!result) {
        perror("malloc");
        return NULL;
    }
    strcpy(result, buffer);
    return result;
}