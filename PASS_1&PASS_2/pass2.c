#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

char *decimalToHex(int decimal) {
    static char hex_string[20];
    sprintf(hex_string, "%X", decimal);
    return hex_string;
}

void main() {
    printf("\nPASS 2.\n");

    char loc[20], label[20], opcode[20], operand[20];
    char optab_opcode[10], optab_machine_code[10];
    char symtab_label[10], symtab_address[10];
    int count = 0, start, end, length = 0;
    bool opcode_found = false;
    bool label_found = false;

    FILE *input, *output, *symtab, *optab;

    input = fopen("interm.txt", "r");
    output = fopen("obj1.txt", "w");
    symtab = fopen("symtab.txt", "r");
    optab = fopen("optab.txt", "r");

    if (input == NULL || output == NULL || symtab == NULL || optab == NULL){
        printf("Error! File not found!");
        exit(0);
    } else {
        printf("All files opened successfully!\n");
    }

    fscanf(input, "%s %s %s %s", loc, label, opcode, operand);
    start = strtol(operand, NULL, 16);

    while (!feof(input)) {
        if (!(strcmp(opcode, "START") == 0 || strcmp(opcode, "END") == 0 || strcmp(opcode, "RESW") == 0 || strcmp(opcode, "RESB") == 0)) {
            count++;
        }

        fscanf(input, "%s %s %s %s", loc, label, opcode, operand);
    }


    end = strtol(loc, NULL, 16);
    printf("END: %d/ START: %d", end, start);

    length = end - start;
    rewind(input);

    fscanf(input, "%s %s %s %s", loc, label, opcode, operand);
    fprintf(output, "H^%s^00%s^0000%d\n", label, operand, length);

    fprintf(output, "T^00%s^%d^", operand, (count * 6) / 2);

    while (!feof(input)) {
        if (strcmp(opcode, "START") == 0 || strcmp(opcode, "END") == 0 || strcmp(opcode, "RESW") == 0 || strcmp(opcode, "RESB") == 0) {
            fscanf(input, "%s %s %s %s", loc, label, opcode, operand);
            continue;
        } else {
            rewind(optab);
            opcode_found = false;
            fscanf(optab, "%s %s", optab_opcode, optab_machine_code);

            while (!feof(optab)) {
                if (strcmp(opcode, optab_opcode) == 0) {
                    opcode_found = true;
                    break;
                }

                fscanf(optab, "%s %s", optab_opcode, optab_machine_code);
            }

            if (opcode_found) {
                rewind(symtab);
                label_found = false;
                fscanf(symtab, "%s %s", symtab_label, symtab_address);

                while (!feof(symtab)) {
                    if (strcmp(symtab_label, operand) == 0) {
                        label_found = true;
                        break;
                    }

                    fscanf(symtab, "%s %s", symtab_label, symtab_address);
                }

                if (label_found) {
                    fprintf(output, "%s%s^", optab_machine_code, symtab_address);
                } else {
                    printf("Error! Label not found in symtab!\n");
                    exit(0);
                }
            } else if (strcmp(opcode, "WORD") == 0) {
                fprintf(output, "00000%s ", operand);
            }
            else if (strcmp(opcode, "BYTE") == 0) {
                for (int i = 2; i < strlen(operand) - 1; i++) {
                    fprintf(output, "%s", decimalToHex(operand[i]));
                }
                fprintf(output, "^");
            } else {
                printf("Error!\n %s", opcode);
                exit(0);
            }
        }
        fscanf(input, "%s %s %s %s", loc, label, opcode, operand);
    }

    fprintf(output, "\nE^00%s\n", decimalToHex(start));

    fclose(input);
    fclose(output);
    fclose(symtab);
    fclose(optab);
}
