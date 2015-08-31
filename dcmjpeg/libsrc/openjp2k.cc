#include "dcmtk/config/osconfig.h"

#include "dcmtk/dcmjpeg/openjp2k.h"
#include <openjpeg-2.1/openjpeg.h>

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#define J2K_CFMT 0
#define JP2_CFMT 1
#define JPT_CFMT 2

namespace {

struct decode_info_t {
    decode_info_t () : codec (), stream (), image (), own () { }

    ~decode_info_t () {
        if (codec)
            opj_destroy_codec (codec);

        if (stream)
            opj_stream_destroy (stream);

        if (image && own)
            opj_image_destroy (image);
    }

    opj_codec_t* codec;
    opj_stream_t* stream;
    opj_image_t* image;

    bool own;
};

#define JP2_RFC3745_MAGIC "\x00\x00\x00\x0c\x6a\x50\x20\x20\x0d\x0a\x87\x0a"
#define JP2_MAGIC "\x0d\x0a\x87\x0a"
#define J2K_CODESTREAM_MAGIC "\xff\x4f\xff\x51"

int
format_of (opj_buffer_info_t* pinfo) {
    int magic = -1;

    if (!pinfo || pinfo->len < 12)
        return magic;

    if (0 == memcmp (pinfo->buf, JP2_RFC3745_MAGIC, 12) ||
        0 == memcmp (pinfo->buf, JP2_MAGIC,         4)) {
        magic = JP2_CFMT;
    }
    else {
        if (0 == memcmp (pinfo->buf, J2K_CODESTREAM_MAGIC, 4))
            magic = J2K_CFMT;
    }

    return magic;
}

const char*
clear_space (OPJ_COLOR_SPACE i) {
    const char* s = "CLRSPC_UNDEFINED";

    switch (i) {
    case OPJ_CLRSPC_SRGB:    s = "OPJ_CLRSPC_SRGB";    break;
    case OPJ_CLRSPC_GRAY:    s = "OPJ_CLRSPC_GRAY";    break;
    case OPJ_CLRSPC_SYCC:    s = "OPJ_CLRSPC_SYCC";    break;
    case OPJ_CLRSPC_UNKNOWN: s = "OPJ_CLRSPC_UNKNOWN"; break;
    default: break;
    }

    return s;
}

void
image_fill (opj_image_t* pimage, const void* psrc, size_t width, size_t height,
            size_t planes, size_t bpp) {

    const size_t n = width * height;

    for (size_t i = 0; i < planes; ++i)
        for (size_t j = 0, k = i; j < n; ++j, k += planes) {
            switch (bpp / 8) {
            case 1:
                pimage->comps [i].data [j] =
                    reinterpret_cast< const unsigned char* > (psrc) [k];
                break;

            case 2:
                pimage->comps [i].data [j] =
                    reinterpret_cast< const unsigned short* > (psrc) [k];
                break;

            case 4:
                pimage->comps [i].data [j] =
                    reinterpret_cast< const unsigned* > (psrc) [k];
                break;

            default:
                break;
            }
        }
}

} // anonymous

void*
ojp2k_decompress (void* pbuf, long buflen, long* plen, int* pcolor) {
    return ojp2k_decompress (0, pbuf, buflen, plen, pcolor);
}

void*
ojp2k_decompress (
    void* pinput, void* pbuf, long buflen, long* plen, int* pcolor) {
    int i, decod_format;
    int width, height;

    opj_buffer_info_t buffer_info = { 0 };

    if (pbuf != 0) {
        buffer_info.len = buflen;
        buffer_info.buf = (OPJ_BYTE*)pbuf;
        buffer_info.cur = buffer_info.buf;
    }

    opj_dparameters_t parameters = { 0 };
    opj_set_default_decoder_parameters (&parameters);

    decod_format = format_of (&buffer_info);

    if (decod_format == -1) {
        fprintf (stderr,"%s:%d: decode format missing\n",__FILE__,__LINE__);
        return 0;
    }

    OPJ_CODEC_FORMAT codec_format;

    if (decod_format == J2K_CFMT)
        codec_format = OPJ_CODEC_J2K;
    else if (decod_format == JP2_CFMT)
        codec_format = OPJ_CODEC_JP2;
    else if (decod_format == JPT_CFMT)
        codec_format = OPJ_CODEC_JPT;
    else
        return 0;

    parameters.decod_format = decod_format;

    decode_info_t decode_info;

    bool failed = true;

    do {
        decode_info.stream = opj_stream_create_buffer_stream (&buffer_info, 1);

        if (0 == decode_info.stream) {
            fprintf (stderr,
                     "%s:%d: NO decode_info.stream\n",
                     __FILE__,__LINE__);
            break;
        }

        decode_info.codec = opj_create_decompress (codec_format);

        if (decode_info.codec == 0) {
            fprintf (stderr, "%s:%d: NO coded\n", __FILE__, __LINE__);
            break;
        }

        if (!opj_setup_decoder (decode_info.codec, &parameters)) {
            fprintf (stderr,
                     "%s:%d:\n\topj_setup_decoder failed\n",
                     __FILE__,__LINE__);
            break;
        }

        if (!opj_read_header (
                decode_info.stream, decode_info.codec, &decode_info.image)) {
            fprintf (stderr,
                     "%s:%d:\n\topj_read_header failed\n",
                     __FILE__,__LINE__);
            break;
        }

        unsigned int x0 = 0, y0 = 0, x1 = 0, y1 = 0;

        if (!opj_set_decode_area (
                decode_info.codec, decode_info.image, x0, y0, x1, y1)) {
            fprintf (stderr,
                     "%s:%d:\n\topj_set_decode_area failed\n",
                     __FILE__,__LINE__);
            break;
        }

        if (!opj_decode (
                decode_info.codec, decode_info.stream, decode_info.image)) {
            fprintf (stderr,
                     "%s:%d:\n\topj_decode failed\n",
                     __FILE__,__LINE__);
            break;
        }

        if (!opj_end_decompress (decode_info.codec, decode_info.stream)) {
            fprintf (stderr,
                     "%s:%d:\n\topj_end_decompress failed\n",
                     __FILE__,__LINE__);
            break;
        }

        failed = false;

    } while (0);

    decode_info.own = failed;

    if (failed)
        return 0;

    decode_info.own = true;

    if (   decode_info.image->color_space != OPJ_CLRSPC_SYCC
        && decode_info.image->numcomps == 3
        && decode_info.image->comps[0].dx == decode_info.image->comps[0].dy
        && decode_info.image->comps[1].dx != 1)
        decode_info.image->color_space = OPJ_CLRSPC_SYCC;
    else
        if (decode_info.image->numcomps <= 2)
            decode_info.image->color_space = OPJ_CLRSPC_GRAY;

    if (decode_info.image->color_space == OPJ_CLRSPC_SYCC) {
        // TODO: re-enable?!
        // color_sycc_to_rgb (decode_info.image);
    }

    if (decode_info.image->icc_profile_buf) {
#if defined (HAVE_LIBLCMS1) || defined (HAVE_LIBLCMS2)
        color_apply_icc_profile (decode_info.image);
#endif
        free (decode_info.image->icc_profile_buf);
        decode_info.image->icc_profile_buf = 0;
        decode_info.image->icc_profile_len = 0;
    }

    width = decode_info.image->comps[0].w;
    height = decode_info.image->comps[0].h;

    long depth = (decode_info.image->comps [0].prec + 7) / 8;

    long n = width * height * decode_info.image->numcomps * depth;

    if (plen)
        plen [0] = n;

    if (!pinput)
        pinput = malloc (n);

    if (pcolor)
        *pcolor = 0;

    if ((    decode_info.image->numcomps >= 3
          && decode_info.image->comps[0].dx == decode_info.image->comps[1].dx
          && decode_info.image->comps[1].dx == decode_info.image->comps[2].dx
          && decode_info.image->comps[0].dy == decode_info.image->comps[1].dy
          && decode_info.image->comps[1].dy == decode_info.image->comps[2].dy
          && decode_info.image->comps[0].prec == decode_info.image->comps[1].prec
          && decode_info.image->comps[1].prec == decode_info.image->comps[2].prec) ||
         (   decode_info.image->numcomps == 2
          && decode_info.image->comps[0].dx == decode_info.image->comps[1].dx
          && decode_info.image->comps[0].dy == decode_info.image->comps[1].dy
          && decode_info.image->comps[0].prec == decode_info.image->comps[1].prec))
    {
        bool has_alpha4, has_alpha2, has_rgb;
        int *red, *green, *blue, *alpha;

        if (pcolor)
            *pcolor = 1;

        alpha = 0;

        has_rgb    = (decode_info.image->numcomps == 3);
        has_alpha4 = (decode_info.image->numcomps == 4);
        has_alpha2 = (decode_info.image->numcomps == 2);

        bool hasAlpha = (has_alpha4 || has_alpha2);

        if (has_rgb) {
            red   = decode_info.image->comps[0].data;
            green = decode_info.image->comps[1].data;
            blue  = decode_info.image->comps[2].data;

            if (has_alpha4)
                alpha = decode_info.image->comps [3].data;
        } else {
            red = green = blue = decode_info.image->comps [0].data;

            if (has_alpha2)
                alpha = decode_info.image->comps[1].data;
        }

        int* pdst = (int*)pinput;

        for (i = 0; i < width * height; ++i) {
            unsigned rc, gc, bc, ac = 255;

            rc = (unsigned char)*red++;
            gc = (unsigned char)*green++;
            bc = (unsigned char)*blue++;

            if (hasAlpha)
                ac = (unsigned char)*alpha++;

            //                  A            R            G          B
            pdst++ [0] = (int)((ac << 24) | (rc << 16) | (gc << 8) | bc);
        }
    } else if (decode_info.image->numcomps == 1) {
        // Grey

        int* psrc = decode_info.image->comps [0].data;

        // 1 component, 8 or 16 bpp
        if (decode_info.image->comps[0].prec <= 8) {
            char* pdst = (char*)pinput;

            for (i = 0; i < width * height; ++i)
                *pdst++ = *psrc++;
        }
        else {
            int* psrc = decode_info.image->comps[0].data;
            short* pdst = (short*)pinput;

            // Disable shift up for signed data: don't know why we are doing this
            for (i = 0; i < width * height; ++i)
                *pdst++ = *psrc++;
        }
    }
    else {
        fprintf (
            stderr,
            "%s:%d : Can show only first component of decode_info.image\n"
            "  components (%d) prec (%d) color_space[%d] (%s)\n"
            "  RECT (%d,%d,%d,%d)\n",
            __FILE__,__LINE__,
            decode_info.image->numcomps,
            decode_info.image->comps[0].prec,
            decode_info.image->color_space,
            clear_space (decode_info.image->color_space),
            decode_info.image->x0,
            decode_info.image->y0,
            decode_info.image->x1,
            decode_info.image->y1);

        for (i = 0; i < decode_info.image->numcomps; ++i) {
            fprintf (stderr,"[%d]dx (%d) dy (%d) w (%d) h (%d) signed (%u)\n",i,
                     decode_info.image->comps[i].dx ,decode_info.image->comps[i].dy,
                     decode_info.image->comps[i].w,decode_info.image->comps[i].h,
                     decode_info.image->comps[i].sgnd);
        }

        // 1 component 8 or 16 bpp
        int* psrc = decode_info.image->comps[0].data;

        if (decode_info.image->comps [0].prec <= 8) {
            char* pdst = (char*)pinput;

            for (i = 0; i < width * height; ++i)
                *pdst++ = *psrc++;
        }
        else {
            int* psrc = decode_info.image->comps [0].data;
            short* pdst = (short*)pinput;

            for (i = 0; i < width * height; ++i)
                *pdst++ = *psrc++;
        }
    }

    return pinput;
}

void*
ojp2k_compress (
    void** ppdst, size_t* pdstlen, void* psrc,
    size_t width, size_t height, size_t planes, size_t bpp) {

    size_t srclen = width * height * planes * (bpp / 8);

    ppdst [0] = 0;
    pdstlen [0] = 0;

    assert (psrc);
    assert (width && height);

    assert (0 == bpp % 8);
    assert (1 == planes || 3 == planes);

    OPJ_COLOR_SPACE color_space;

    if (1 == planes)
        color_space = OPJ_CLRSPC_GRAY;
    else if (3 == planes)
        color_space = OPJ_CLRSPC_SRGB;
    else
        return 0;

    opj_cparameters_t parameters = { 0 };
    opj_set_default_encoder_parameters (&parameters);

    parameters.tcp_numlayers = 1;
    parameters.cp_disto_alloc = 1;

    // Lossless rate
    parameters.tcp_rates [0] = 0;

    int subsampling_dx = parameters.subsampling_dx;
    int subsampling_dy = parameters.subsampling_dy;

    opj_image_cmptparm_t cmptparm [3] = { 0 };

    for (size_t i = 0; i < planes; i++) {
        cmptparm [i].prec = cmptparm [i].bpp = bpp;

        cmptparm [i].dx = subsampling_dx;
        cmptparm [i].dy = subsampling_dy;

        cmptparm [i].w = width;
        cmptparm [i].h = height;
    }

    opj_image_t* pimage = opj_image_create (planes, cmptparm, color_space);

    if (0 == pimage)
        return 0;

    pimage->x0 = parameters.image_offset_x0;
    pimage->y0 = parameters.image_offset_y0;

    pimage->x1 = parameters.image_offset_x0 + (width  - 1) * subsampling_dx + 1;
    pimage->y1 = parameters.image_offset_y0 + (height - 1) * subsampling_dy + 1;

    image_fill (pimage, psrc, width, height, planes, bpp);

    parameters.cod_format = 0;

    opj_codec_t* pcodec = opj_create_compress (OPJ_CODEC_J2K);
    opj_setup_encoder (pcodec, &parameters, pimage);

    opj_buffer_info_t opj_buffer_info = { 0 };

    opj_stream_t* pstream = opj_stream_create_buffer_stream (
        &opj_buffer_info, 0);

    if (pstream &&
        opj_start_compress (pcodec, pimage, pstream) &&
        opj_encode (pcodec, pstream) &&
        opj_end_compress (pcodec, pstream)) {

        ppdst [0] = opj_buffer_info.buf;
        pdstlen [0] = opj_buffer_info.len;
    }

    return opj_image_destroy (pimage), ppdst [0];
}
