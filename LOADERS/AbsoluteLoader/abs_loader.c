#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>

void main() {
    FILE *inp, *out;
    inp = fopen("input.txt", "r");
    out = fopen("out.txt", "w");

    char line[100];
    fgets(line, 100, inp);

    while (!feof(inp)) {
        int i = 0;
        if (line[0] == 'T') {
            int start = 0;
            char *token = strtok(line, "^");
            while (token != NULL) {
                if (i == 1) {
                    start = strtol(token, NULL, 16);
                } else if (i != 0 && i != 2) {
                    fprintf(out, "%X:%s\n", start, token);
                    start += strlen(token) / 2;
                }
                token = strtok(NULL, "^");
                i++;
            }
        }
        fgets(line, 100, inp);
    }
}
