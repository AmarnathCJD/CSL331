#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 100

int hex_to_int(char *hex) {
    return (int)strtol(hex, NULL, 16);
}

void int_to_hex(int num, char *hex, int length) {
    snprintf(hex, length + 1, "%0*X", length, num);
}

char* addByPosition(char* num, int toAdd, int length) {
    char* last = num + strlen(num) - length;
    int new_numz = hex_to_int(last) + toAdd;
    char new_numx[10];
    int_to_hex(new_numz, new_numx, length);
    
    char* new_num = malloc(strlen(num) + 1);

    strncpy(new_num, num, strlen(num) - length);
    strcpy(new_num + strlen(num) - length, new_numx);
    return new_num;
}


char HEADER[MAX_LINE_LENGTH];
char END[MAX_LINE_LENGTH];
char text_records[MAX_LINE_LENGTH][MAX_LINE_LENGTH];
int text_record_count = 0;

void processHeader(char *line, int new_start, FILE *outFile) {
    char name[7];
    int length, old_start;
    sscanf(line, "H %s %X %d", name, &old_start, &length);
    
    int new_length = length + new_start;
    snprintf(HEADER, MAX_LINE_LENGTH, "H %s %06X %d", name, new_start, length);
}

void processText(char *line, int offset, FILE *outFile) {
    char rest_of_line[MAX_LINE_LENGTH];
    int address, size;
    sscanf(line, "T %X %X %[^\n]", &address, &size, rest_of_line);
    snprintf(text_records[text_record_count++], MAX_LINE_LENGTH, "T %06X %X %s", address + offset, size, rest_of_line);
}

void processModification(char *line, FILE *outFile, int new_start) {
    char rest_of_line[MAX_LINE_LENGTH];
    int address, size;
    sscanf(line, "M %X %X %[^\n]", &address, &size, rest_of_line);

    FILE *file = fopen("input.txt", "r");
    if (!file) {
        printf("Error opening file.\n");
        return;
    }

    char line_buffer[MAX_LINE_LENGTH];
    int count = 0;
    char *to_replace = NULL;

    while (fgets(line_buffer, sizeof(line_buffer), file)) {
        if (line_buffer[0] == 'T' && count < address) {
            int text_address, text_size;
            char text_object_code[MAX_LINE_LENGTH];
            sscanf(line_buffer, "T %X %X %[^\n]", &text_address, &text_size, text_object_code);
            char *token = strtok(text_object_code, " ");
            count += strlen(token) / 2;

            while (count < address) {
                token = strtok(NULL, " ");
                count += strlen(token) / 2;
            }
            to_replace = token;
            break;
        }
    }

    fclose(file);

    if (to_replace != NULL) {
       char* new_value = addByPosition(to_replace, new_start, size);
        for (int i = 0; i < MAX_LINE_LENGTH; i++) {
            // REPLACEit,
            if (strstr(text_records[i], to_replace) != NULL) {
                char *pos = strstr(text_records[i], to_replace);
                int index = pos - text_records[i];
              
                for (int j = 0; j < size; j++) {
                    text_records[i][index + j] = new_value[j];
                }

                break;
            }
        }
    }
}

int main() {
    printf("Relocating Loader...\n");

    FILE *inFile = fopen("input.txt", "r");
    FILE *outFile = fopen("output.txt", "w");
    if (inFile == NULL || outFile == NULL) {
        printf("Error opening file.\n");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    int new_start;

    printf("Enter new starting address (hex): ");
    scanf("%X", &new_start);

    // Read input file line by line
    while (fgets(line, sizeof(line), inFile)) {
        if (line[0] == 'H') {
            processHeader(line, new_start, outFile);
        } else if (line[0] == 'T') {
            processText(line, new_start - 0x1000, outFile); // change 0x1000 to the original starting address
        } else if (line[0] == 'M') {
            processModification(line, outFile, new_start);
        } else if (line[0] == 'E') {
            snprintf(END, MAX_LINE_LENGTH, "E %06X", new_start);
        }
    }

    fprintf(outFile, "%s\n", HEADER);
    for (int i = 0; i < text_record_count; i++) {
        fprintf(outFile, "%s\n", text_records[i]);
    }
    fprintf(outFile, "%s", END);

    fclose(inFile);
    fclose(outFile);

    printf("Relocation completed. Output written to output.txt\n");
    return 0;
}
