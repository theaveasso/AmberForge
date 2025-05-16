#include "amber_forge.h"

#include "af_log.h"
#include "af_memory.h"

#include <stdlib.h>

char *afioReadFile(const char *path, AFarena *a) {
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        AFERROR("(afioReadFile) failed to open file %s\n", path);
        abort();
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    char *buffer = afArenaAlloc(a, size + 1);
    if (!buffer)
    {
        AFERROR("(afioReadFile) failed to alloacate memory for buffer %s\n", path);
        fclose(file);
        abort();
    }

    const size_t read = fread(buffer, 1, size, file);
    buffer[read]      = '\0';
    fclose(file);
    return buffer;
}
