#include "res.h"
#include "mem.h"

char *read_entire_file(FILE *stream)
{
    fseek(stream, 0, SEEK_END);
    size_t size = ftell(stream);
    fseek(stream, 0, SEEK_SET);
    char *buffer = stack_push_alloc(&temp_mem, size);
    fread(buffer, 1, size, stream);
    buffer[size] = '\0';
    return buffer;
}
