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
 *
 *  Author:  Norbert Olges, Marco Eichelberg
 *
 *  Purpose: decompression routines of the IJG JPEG library configured for 16 bits/sample.
 *
 *  Last Update:      $Author: lpysher $
 *  Update Date:      $Date: 2006/03/01 20:15:44 $
 *  Source File:      $Source: /cvsroot/osirix/osirix/Binaries/dcmtk-source/dcmjpeg/djdijg16.h,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 */

#ifndef DJDIJP2K_H
#define DJDIJP2K_H

#include "dcmtk/dcmjpeg/djdecabs.h" /* for class DJDecoder */

class DJCodecParameter;

/** this class encapsulates the decompression routines of the
 *  JPEG-2000 library, OpenJPEG.
 */
class DJDecompressJP2k : public DJDecoder
{
public:

    /** constructor
     *  @param cp codec parameters
     *  @param isYBR flag indicating if DICOM photometric interpretation is YCbCr
     */
    DJDecompressJP2k(const DJCodecParameter& cp, OFBool isYBR);

    /// destructor
    virtual ~DJDecompressJP2k();

    /** initializes internal object structures.
     *  Must be called before a new frame is decompressed.
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition init() {
        return EC_Normal;
    }

    /** suspended decompression routine. Decompresses a JPEG frame
     *  until finished or out of data. Can be called with new data
     *  until a frame is complete.
     *  @param compressedFrameBuffer pointer to compressed input data, must not be NULL
     *  @param compressedFrameBufferSize size of buffer, in bytes
     *  @param uncompressedFrameBuffer pointer to uncompressed output data, must not be NULL.
     *     This buffer must not change between multiple decode() calls for a single frame.
     *  @param uncompressedFrameBufferSize size of buffer, in bytes (!)
     *     Buffer must be large enough to contain a complete frame.
     *  @param isSigned OFTrue, if uncompressed pixel data is signed, OFFalse otherwise
     *  @return EC_Normal if successful, EC_Suspend if more data is needed, an error code otherwise.
     */
    virtual OFCondition decode(
        Uint8 *compressedFrameBuffer,
        Uint32 compressedFrameBufferSize,
        Uint8 *uncompressedFrameBuffer,
        Uint32 uncompressedFrameBufferSize,
        OFBool isSigned);

    /** returns the number of bytes per sample that will be written when decoding.
     */
    virtual Uint16 bytesPerSample() const
    {
        return sizeof(Uint16);
    }

    /** after successful compression,
     *  returns the color model of the decompressed image
     */
    virtual EP_Interpretation getDecompressedColorModel() const
    {
        return decompressedColorModel;
    }

    /** callback function used to report warning messages and the like.
     *  Should not be called by user code directly.
     */
    virtual void outputMessage() const;

private:
    DJDecompressJP2k (const DJDecompressJP2k&);
    DJDecompressJP2k& operator= (const DJDecompressJP2k&);

private:
    /// codec parameters
    const DJCodecParameter *cparam;

    /// Flag indicating if DICOM photometric interpretation is YCbCr
    OFBool dicomPhotometricInterpretationIsYCbCr;

    /// color model after decompression
    EP_Interpretation decompressedColorModel;
};

#endif
