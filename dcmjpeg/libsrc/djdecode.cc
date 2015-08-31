/*
 *
 *  Copyright (C) 1997-2010, OFFIS e.V.
 *  All rights reserved.  See COPYRIGHT file for details.
 *
 *  This software and supporting documentation were developed by
 *
 *    OFFIS e.V.
 *    R&D Division Health
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *
 *  Module:  dcmjpeg
 *
 *  Author:  Marco Eichelberg
 *
 *  Purpose: singleton class that registers decoders for all supported JPEG processes.
 *
 */

#include "dcmtk/config/osconfig.h"
#include "dcmtk/dcmjpeg/djdecode.h"

#include "dcmtk/dcmdata/dccodec.h"  /* for DcmCodecStruct */
#include "dcmtk/dcmjpeg/djdecbas.h"
#include "dcmtk/dcmjpeg/djdecext.h"
#include "dcmtk/dcmjpeg/djdecsps.h"
#include "dcmtk/dcmjpeg/djdecpro.h"
#include "dcmtk/dcmjpeg/djdecsv1.h"
#include "dcmtk/dcmjpeg/djdeclol.h"
#include "dcmtk/dcmjpeg/djcparam.h"

#if defined (WITH_OPENJPEG2)
#  include "dcmtk/dcmjpeg/djdec2k.h"
#endif // WITH_OPENJPEG2

// initialization of static members
OFBool DJDecoderRegistration::registered                  = OFFalse;
DJCodecParameter *DJDecoderRegistration::cp               = NULL;
DJDecoderBaseline *DJDecoderRegistration::decbas          = NULL;
DJDecoderExtended *DJDecoderRegistration::decext          = NULL;
DJDecoderSpectralSelection *DJDecoderRegistration::decsps = NULL;
DJDecoderProgressive *DJDecoderRegistration::decpro       = NULL;
DJDecoderP14SV1 *DJDecoderRegistration::decsv1            = NULL;
DJDecoderLossless *DJDecoderRegistration::declol          = NULL;
DJDecoderJP2k *DJDecoderRegistration::dec2k				  = NULL;
DJDecoderJP2kLossLess *DJDecoderRegistration::dec2kLossLess	= NULL;

void DJDecoderRegistration::registerCodecs(
    E_DecompressionColorSpaceConversion pDecompressionCSConversion,
    E_UIDCreation pCreateSOPInstanceUID,
    E_PlanarConfiguration pPlanarConfiguration,
    OFBool predictor6WorkaroundEnable)
{
  if (! registered)
  {
    cp = new DJCodecParameter(
      ECC_lossyYCbCr, // ignored, compression only
      pDecompressionCSConversion,
      pCreateSOPInstanceUID,
      pPlanarConfiguration,
      predictor6WorkaroundEnable);
    if (cp)
    {
      // baseline JPEG
      decbas = new DJDecoderBaseline();
      if (decbas) DcmCodecList::registerCodec(decbas, NULL, cp);

      // extended JPEG
      decext = new DJDecoderExtended();
      if (decext) DcmCodecList::registerCodec(decext, NULL, cp);

      // spectral selection JPEG
      decsps = new DJDecoderSpectralSelection();
      if (decsps) DcmCodecList::registerCodec(decsps, NULL, cp);

      // progressive JPEG
      decpro = new DJDecoderProgressive();
      if (decpro) DcmCodecList::registerCodec(decpro, NULL, cp);

      // lossless SV1 JPEG
      decsv1 = new DJDecoderP14SV1();
      if (decsv1) DcmCodecList::registerCodec(decsv1, NULL, cp);

      // lossless JPEG
      declol = new DJDecoderLossless();
      if (declol) DcmCodecList::registerCodec(declol, NULL, cp);

#if defined (WITH_OPENJPEG2)
      // JPEG 2K
      dec2k = new DJDecoderJP2k();
      if (dec2k) DcmCodecList::registerCodec(dec2k, NULL, cp);

       // JPEG 2K LossLess
      dec2kLossLess = new DJDecoderJP2kLossLess();
      if (dec2kLossLess) DcmCodecList::registerCodec(dec2kLossLess, NULL, cp);
#endif // WITH_OPENJPEG2

      registered = OFTrue;
    }
  }
}

void DJDecoderRegistration::cleanup()
{
  if (registered)
  {
    DcmCodecList::deregisterCodec(decbas);
    delete decbas;
    DcmCodecList::deregisterCodec(decext);
    delete decext;
    DcmCodecList::deregisterCodec(decsps);
    delete decsps;
    DcmCodecList::deregisterCodec(decpro);
    delete decpro;
    DcmCodecList::deregisterCodec(decsv1);
    delete decsv1;
    DcmCodecList::deregisterCodec(declol);
    delete declol;

#if defined (WITH_OPENJPEG2)
    DcmCodecList::deregisterCodec (dec2k);
    delete dec2k;

    DcmCodecList::deregisterCodec (dec2kLossLess);
    delete dec2kLossLess;
#endif (WITH_OPENJPEG2)

    delete cp;

    registered = OFFalse;

#ifdef DEBUG
    // not needed but useful for debugging purposes
    decbas = NULL;
    decext = NULL;
    decsps = NULL;
    decpro = NULL;
    decsv1 = NULL;
    declol = NULL;
    cp     = NULL;

#  if defined (WITH_OPENJPEG2)
    dec2k = NULL;
    dec2kLossLess = NULL;
#  endif (WITH_OPENJPEG2)

#endif

  }
}
