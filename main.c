#include "src/disk.c"
#include "src/fs.c"
#include <stdio.h>

#include "utils/utils.c"
#include "utils/tests.c"

int main() {
    reset_disk();
    init_fat();

    test_dir_entry();

    printf("Sucesso!\n");
    return 0;
}