
#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djdijp2k.h"
#include "dcmtk/dcmjpeg/djcparam.h"
#include "dcmtk/ofstd/ofconsol.h"
#include "dcmtk/dcmjpeg/openjp2k.h"

#include <sys/types.h>

#define INCLUDE_CSTDIO
#define INCLUDE_CSETJMP

#include "dcmtk/ofstd/ofstdinc.h"

DJDecompressJP2k::DJDecompressJP2k (const DJCodecParameter& cp, OFBool isYBR)
    : DJDecoder()
    , cparam(&cp)
    , dicomPhotometricInterpretationIsYCbCr(isYBR)
    , decompressedColorModel(EPI_Unknown)
{
}

DJDecompressJP2k::~DJDecompressJP2k()
{ }


OFCondition DJDecompressJP2k::decode(
    Uint8* compressedFrameBuffer,
    Uint32 compressedFrameBufferSize,
    Uint8* uncompressedFrameBuffer,
    Uint32 uncompressedFrameBufferSize,
    OFBool isSigned)
{
    int colorModel;
    long uncompressedSize = 0;

    if (0 == ojp2k_decompress (
        uncompressedFrameBuffer,
        compressedFrameBuffer, compressedFrameBufferSize,
        &uncompressedSize, &colorModel))
        return EC_MemoryExhausted;

    if( colorModel == 1)
        decompressedColorModel = (EP_Interpretation)EPI_RGB;

    return EC_Normal;
}

void DJDecompressJP2k::outputMessage() const { }
