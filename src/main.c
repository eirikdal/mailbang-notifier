#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#include "inc/config.h"

#ifndef MAIN_H
#include "inc/main.h"
#define MAIN_H
#endif

int lsdir(const char* dirarg) {
    DIR* dir;
    struct dirent *ent;
    if ((dir = opendir (dirarg)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            printf ("%s\n", ent->d_name);
        }
        closedir (dir);
    } else {
        perror ("");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int rdconf(const char* configFile) {
    FILE *ifp;
    char *mode = "r+";
    char key[100];
    char val[100];

    ifp = fopen(configFile, mode);

    if (ifp == NULL) {
        fprintf(stderr, "Can't open config file %s!\n", configFile);
        exit(EXIT_FAILURE);
    }

    while (fscanf(ifp, "%s %s", key, val) != EOF) {
        printf("%s %d\n", key, val);
    }

    return EXIT_SUCCESS;
}

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        printf( "usage: %s <mailbox>", argv[0] );
        return 0;
    }

    rdconf(argv[1]);
    /* lsdir (argv [1]); */

    return EXIT_SUCCESS;
}

