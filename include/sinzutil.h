#ifndef ZLIB_UTIL_H
#define ZLIB_UTIL_H

#include <stdint.h>

int zDef(uint8_t *source, int source_size, uint8_t *dest, int &dest_size, int comp_level);

int zInf(uint8_t *source, int source_size, uint8_t *dest, int &dest_size);


#endif
