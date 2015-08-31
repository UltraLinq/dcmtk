#ifndef OPENJP2K_H
#define OPENJP2K_H

#include "dcmtk/config/osconfig.h"

#include <cstddef>

void*
ojp2k_decompress (void* jp2Data, long jp2DataSize, long *decompressedBufferSize, int* colorModel);

void*
ojp2k_decompress (void* inputBuffer, void* jp2Data, long jp2DataSize, long *decompressedBufferSize, int *colorModel);

void*
ojp2k_compress (void** ppdst, size_t* pdstlen, void* psrc, size_t width, size_t height, size_t planes, size_t bpp);

#endif // OPENJP2K_H
