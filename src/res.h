#ifndef _RES_H
#define _RES_H

#include <stdlib.h>
#include <stdio.h>

#define RES_FILE "res"
#define JOIN_PATH(x, y) x "/" y

// allocates memory in temp_memory so remember to pop it
char *read_entire_file(FILE *stream);

#endif