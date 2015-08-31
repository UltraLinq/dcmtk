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
 *  Author:  Marco Eichelberg, Norbert Olges
 *
 *  Purpose: Codec class for encoding JPEG Lossless (8/12/16-bit)
 *
 *  Last Update:      $Author: lpysher $
 *  Update Date:      $Date: 2006/03/01 20:15:44 $
 *  Source File:      $Source: /cvsroot/osirix/osirix/Binaries/dcmtk-source/dcmjpeg/djenclol.cc,v $
 *  CVS/RCS Revision: $Revision: 1.1 $
 *  Status:           $State: Exp $
 *
 *  CVS/RCS Log at end of file
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djcparam.h"
#include "dcmtk/dcmjpeg/djenc2k.h"
#include "dcmtk/dcmjpeg/djrploss.h"
#include "dcmtk/dcmjpeg/djrploss.h"
#include "dcmtk/dcmjpeg/djeijg2k.h"

DJEncoder2K::DJEncoder2K()
    : DJCodecEncoder()
{
}


DJEncoder2K::~DJEncoder2K()
{
}


E_TransferSyntax DJEncoder2K::supportedTransferSyntax() const
{
    return EXS_JPEG2000;
}


OFBool DJEncoder2K::isLosslessProcess() const
{
    return OFFalse;
}


void DJEncoder2K::createDerivationDescription(
    const DcmRepresentationParameter * toRepParam,
    const DJCodecParameter * /* cp */ ,
    Uint8 /* bitsPerSample */ ,
    double ratio,
    OFString& derivationDescription) const
{
    DJ_RPLossy defaultRP;
    const DJ_RPLossy *rp = toRepParam ? (const DJ_RPLossy *)toRepParam : &defaultRP ;
    char buf[64];

    derivationDescription =  "Lossy compression with JPEG 2K ";
    derivationDescription += ", quality factor ";
    sprintf(buf, "%u", rp->getQuality());
    derivationDescription += buf;
    derivationDescription += ", compression ratio ";
    appendCompressionRatio(derivationDescription, ratio);
}


DJEncoder *DJEncoder2K::createEncoderInstance(
    const DcmRepresentationParameter * toRepParam,
    const DJCodecParameter *cp,
    Uint8 bitsPerSample) const
{
    DJ_RPLossy defaultRP;
    const DJ_RPLossy *rp = toRepParam ? (const DJ_RPLossy *)toRepParam : &defaultRP ;
    DJEncoder * result = NULL;

    result = new DJCompressJP2K(*cp, EJM_JP2K_lossy, rp->getQuality(), bitsPerSample);

    return result;
}

// *************

DJEncoder2KLossLess::DJEncoder2KLossLess()
    : DJCodecEncoder()
{
}


DJEncoder2KLossLess::~DJEncoder2KLossLess()
{
}


E_TransferSyntax DJEncoder2KLossLess::supportedTransferSyntax() const
{
    return EXS_JPEG2000LosslessOnly;
}


OFBool DJEncoder2KLossLess::isLosslessProcess() const
{
    return OFTrue;
}


void DJEncoder2KLossLess::createDerivationDescription(
    const DcmRepresentationParameter * toRepParam,
    const DJCodecParameter * /* cp */ ,
    Uint8 /* bitsPerSample */ ,
    double ratio,
    OFString& derivationDescription) const
{
    derivationDescription =
        OFString ("LossLess compression with JPEG 2K") +=
        ", compression ratio ";

    appendCompressionRatio (derivationDescription, ratio);
}


DJEncoder *DJEncoder2KLossLess::createEncoderInstance(
    const DcmRepresentationParameter* prep,
    const DJCodecParameter* pcodec,
    Uint8 bitsPerSample) const
{
    static const DJ_RPLossy default_;

    Uint8 quality = prep
        ? reinterpret_cast< const DJ_RPLossy* > (prep)->getQuality ()
        : default_.getQuality ();

    return new DJCompressJP2K (*pcodec, EJM_JP2K_lossless, quality, bitsPerSample);
}
