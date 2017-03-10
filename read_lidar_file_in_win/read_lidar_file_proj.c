#include <stdio.h>
#include <stdlib.h>
#include "read_lidar_file.h"


int main(int argc, char *argv[])
{
    if( argc < 2 )
    {
        printf("usage: %s %s/n", argv[0], "dir_path");
        exit(1);
    }

    Lidar_TransferAllDataFilesInDir(argv[1]);

    system("PAUSE");
    return 0;
}



