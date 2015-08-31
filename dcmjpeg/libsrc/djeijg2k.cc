/*
 *
 *  Copyright (C) 1997-2005, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  dcmjpeg
 *  Author:  anonymous
 *
 *  Purpose: compression routines of the JPEG-2000 library.
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/ofstd/ofconsol.h"
#include "dcmtk/dcmdata/dcerror.h"
#include "dcmtk/dcmjpeg/djeijg2k.h"
#include "dcmtk/dcmjpeg/djcparam.h"
#include "dcmtk/dcmjpeg/openjp2k.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSETJMP

#include "dcmtk/ofstd/ofstdinc.h"

DJCompressJP2K::DJCompressJP2K (
    const DJCodecParameter& cp, EJ_Mode mode, Uint8 theQuality, Uint8 theBitsPerSample)
    : DJEncoder()
    , cparam(&cp)
    , quality(theQuality)
    , bitsPerSampleValue(theBitsPerSample)
    , modeofOperation(mode)
{ }

DJCompressJP2K::~DJCompressJP2K ()
{ }

/* virtual */ Uint16
DJCompressJP2K::bytesPerSample () const {
    return bitsPerSampleValue <= 8 ? 1 : 2;
}

/* virtual */ Uint16
DJCompressJP2K::bitsPerSample () const {
    return bitsPerSampleValue;
}

/* virtual */ OFCondition
DJCompressJP2K::encode(
    Uint16 columns,
    Uint16 rows,
    EP_Interpretation colorSpace,
    Uint16 samplesPerPixel,
    Uint8 * image_buffer,
    Uint8 * & to,
    Uint32 & length,
    Uint8 pixelRepresentation,
    double minUsed, double maxUsed)
{
    assert (0 == pixelRepresentation);

    return do_encode (
        columns, rows, colorSpace, samplesPerPixel, image_buffer,
        to, length, 8, pixelRepresentation, minUsed, maxUsed);
}

/* virtual */ OFCondition
DJCompressJP2K::encode (
    Uint16  columns ,
    Uint16  rows ,
    EP_Interpretation  interpr ,
    Uint16  samplesPerPixel ,
    Uint16 *  image_buffer ,
    Uint8 *&  to ,
    Uint32 &  length,
    Uint8 pixelRepresentation,
    double minUsed, double maxUsed)
{
    assert (0 == pixelRepresentation);

    return do_encode (
        columns, rows, interpr, samplesPerPixel, (Uint8*)image_buffer,
        to, length, 16, pixelRepresentation, minUsed, maxUsed);
}

/* virtual */ OFCondition
DJCompressJP2K::do_encode (
    Uint16 columns,
    Uint16 rows,
    EP_Interpretation interpretation,
    Uint16 samplesPerPixel,
    Uint8 *image_buffer,
    Uint8 *&to,
    Uint32 &length,
    Uint8 bitsAllocated,
    Uint8 /* ignored */,
    double /* ignored */, double /* ignored */) {

    void* pdst = 0;
    size_t dstlen = 0;

    switch (interpretation) {
    case EPI_Monochrome1:
    case EPI_Monochrome2:
    case EPI_RGB:
        pdst = ojp2k_compress (
            &pdst, &dstlen, image_buffer, columns, rows,
            samplesPerPixel, bitsAllocated);
        break;

    case EPI_Unknown:
    case EPI_Missing:
    case EPI_PaletteColor:
    case EPI_HSV:
    case EPI_ARGB:
    case EPI_CMYK:
    case EPI_YBR_Full:
    case EPI_YBR_Full_422:
    case EPI_YBR_Partial_422:
        return EC_IllegalCall;
    }

    if (pdst) {
        to = reinterpret_cast< Uint8* > (pdst);
        length = dstlen;
    }

    return pdst ? EC_Normal : EC_MemoryExhausted;
}
