#include "src/disk.c"
#include "src/fs.c"
#include <stdio.h>

#include "utils/utils.c"
#include "utils/tests.c"

int main() {
    reset_disk();
    init_fat();

    record_external_file();

    //printf("%s\n", buffer);
    //printf(sizeof())

    printf("Sucesso!\n");
    return 0;
}