#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#define RED "\033[31m"
#define NORMAL "\033[0m"

__attribute__((noreturn)) void panic(char *message) {
    fprintf(stderr, RED);
    fprintf(stderr, "%s", message);
    exit(1);
}
