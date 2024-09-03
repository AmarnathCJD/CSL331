#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char *decimalToHex(int decimal) {
    static char hex_string[10];
    sprintf(hex_string, "%X", decimal);
    return hex_string;
}

int main() {
    printf("PASS 1\n");

    char saved_labels[10][10];
    int saved_labels_index = 0;

    char label[10], opcode[10], operand[10];
    char optab_opcode[10], machine_code[10];
    int starting_address, location_counter;
    bool opcode_found = false;

    FILE *input_file, *optab_file, *symtab_file, *output_file;

    input_file = fopen("input.txt", "r");
    optab_file = fopen("optab.txt", "r");
    symtab_file = fopen("symtab.txt", "w+");
    output_file = fopen("intermediate.txt", "w");

    if (input_file == NULL || optab_file == NULL || symtab_file == NULL || output_file == NULL) {
        printf("Error! opening file");
        exit(1);
    } else {
        printf("All file opened successfully\n");
    }

    fscanf(input_file, "%s %s %s", label, opcode, operand);

    if (strcmp(opcode, "START") == 0) {
        starting_address = strtol(operand, NULL, 16);
        location_counter = starting_address;
    } else {
        location_counter = 0;
        starting_address = 0;
    }

    printf("\nOUTPUT:\n\n");

    fprintf(output_file, "%s\t%s\t%s\t%s\n", "-", label, opcode, operand);
    printf("%s\t%s\t%s\t%s\n", "-", label, opcode, operand);

    fscanf(input_file, "%s %s %s", label, opcode, operand);
    while (strcmp(opcode, "END") != 0) {
        if (strcmp(label, "-") != 0) {
            for (int i = 0; i < 10; i++) {
                if (strcmp(saved_labels[i], label) == 0) {
                    printf("Error! Duplicate label: %s found at line %d\n", label, i + 1);
                    exit(1);
                }
            }

            strcpy(saved_labels[saved_labels_index], label);
            saved_labels_index++;

            fprintf(symtab_file, "%s\t%s\n", label, decimalToHex(location_counter));
        }

        rewind(optab_file);
        opcode_found = false;
        fscanf(optab_file, "%s %s", optab_opcode, machine_code);

        while (!feof(optab_file)) {
            if (strcmp(optab_opcode, opcode) == 0) {
                opcode_found = true;
                break;
            }
            fscanf(optab_file, "%s %s", optab_opcode, machine_code);
        }

        fprintf(output_file, "%s\t%s\t%s\t%s\n", decimalToHex(location_counter), label, opcode, operand);
        printf("%s\t%s\t%s\t%s\n", decimalToHex(location_counter), label, opcode, operand);

        if (opcode_found) {
            location_counter += 3;
        } else if (strcmp(opcode, "BYTE") == 0) {
            location_counter += strlen(operand) - 3;
        } else if (strcmp(opcode, "RESB") == 0) {
            location_counter += atoi(operand);
        } else if (strcmp(opcode, "WORD") == 0) {
            location_counter += 3;
        } else if (strcmp(opcode, "RESW") == 0) {
            location_counter += 3 * atoi(operand);
        } else {
            printf("Error! Invalid opcode\n");
            printf("opcode: %s\n", opcode);
            exit(1);
        }

        fscanf(input_file, "%s %s %s", label, opcode, operand);
    }

    fprintf(output_file, "%s\t%s\t%s\t%s\n", decimalToHex(location_counter), label, opcode, operand);
    printf("%s\t%s\t%s\t%s\n", decimalToHex(location_counter), label, opcode, operand);

    printf("\nSize of the program: %s\n", decimalToHex(location_counter - starting_address));

    fclose(input_file);
    fclose(optab_file);
    fclose(symtab_file);
    fclose(output_file);

    return 0;
}
