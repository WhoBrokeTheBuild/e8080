#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "disasm.h"
#include "run.h"

void usage() 
{
    printf(
        "USAGE: e8080 [OPTIONS] <executable>\n"
        "\n"
        "Default: Emulate the given 8080 executable\n"
        "OPTIONS"
        " -d    Disassemble\n"
    );
}

int main(int argc, char** argv) 
{
    bool oDisassemble = false;
    const char * filename = NULL;

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            for (int c = 1; c < strlen(argv[i]); ++c) {
                if (argv[i][c] == 'd') {
                    oDisassemble = true;
                }
            }
        }
        else {
            filename = argv[i];
        }
    }

    if (!filename) {
        usage();
        return 0;
    }

    uint8_t * program = NULL;
    FILE * fp = fopen(filename, "rb");

    if (!fp) {
        fprintf(stderr, "Failed to open %s\n", argv[1]);
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    long programSize = ftell(fp);
    rewind(fp);

    program = (uint8_t *)malloc(programSize);
    long bytesRead = fread(program, programSize, 1, fp);
    
    fclose(fp);

    if (bytesRead == 0) {
        fprintf(stderr, "Faield to read entire file \n");
        free(program);
        return 1;
    }

    if (oDisassemble) {
        disasm(program, programSize);
    }
    else {
        run(program, programSize);
    }

    free(program);

    return 0;
}
