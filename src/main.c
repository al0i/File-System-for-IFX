#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "virtual_disk.c"

int main()
{
    FILE *disk = init_disk();

    if (initialize_virtual_disk(disk) != 0)
    {
        fclose(disk);
        return -1;
    }

    if (debug_write_block_labels(disk) != 0)
    {
        fclose(disk);
        return -1;
    }

    if (write_root_block(disk) != 0)
    {
        fclose(disk);
        return -1;
    }

    return 0;
}