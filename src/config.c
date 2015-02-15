#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#ifndef CONFIG_H
#include "inc/config.h"
#define CONFIG_H


typedef struct
{
    int version;
    const char* key;
    const char* value;
} configuration;
#endif

static struct configuration* parse(struct dirent* dirent)
{
    return (struct configuration*) 0;
}
