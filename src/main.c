#include <stdio.h>
#include <stdlib.h>

#include "ifx_fs.h"

int main()
{
    IFXFileSystem fs;

    if (ifx_init(&fs) != 0) {
        return -1;
    }

    if (ifx_mount(&fs) != 0) {
        ifx_close(&fs);
        return -1;
    }

    if (ifx_close(&fs) != 0) {
        return -1;
    }

    return 0;
}
