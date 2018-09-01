#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    FILE *target;

    if (argc != 2) {
        fprintf(stderr, "Usage: mcc [options] file...\n");
        return 1;
    }

    target = fopen("tmp.s", "w");

    fprintf(target, ".intel_syntax noprefix\n");
    fprintf(target, ".globl _main\n");
    fprintf(target, "_main:\n");
    fprintf(target, "  mov rax, %d\n", atoi(argv[1]));
    fprintf(target, "  ret\n");

    fclose(target);

    return 0;
}
