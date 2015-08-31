# declare additional include directories
INCLUDE_DIRECTORIES(
  ${dcmjpeg_SOURCE_DIR}/include
  ${ofstd_SOURCE_DIR}/include
  ${oflog_SOURCE_DIR}/include
  ${dcmdata_SOURCE_DIR}/include
  ${dcmimgle_SOURCE_DIR}/include
  ${dcmimage_SOURCE_DIR}/include
  ${dcmjpeg_SOURCE_DIR}/libijg8
  ${dcmjpeg_SOURCE_DIR}/libijg12
  ${dcmjpeg_SOURCE_DIR}/libijg16
  ${ZLIB_INCDIR})

IF(DCMTK_WITH_OPENJPEG2)
  INCLUDE_DIRECTORIES(BEFORE ${LIBOPENJPEG2_INCLUDE_DIR})
ENDIF(DCMTK_WITH_OPENJPEG2)

get_property(CURRENT_INCLUDES
  DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)

SET(DCMJPEG_SOURCES
  ddpiimpl.cc dipijpeg.cc djcodecd.cc djcodece.cc djcparam.cc
  djdecbas.cc djdecext.cc djdeclol.cc djdecode.cc djdecpro.cc
  djdecsps.cc djdecsv1.cc djdijg12.cc djdijg16.cc djdijg8.cc
  djeijg12.cc djeijg16.cc djeijg8.cc djencbas.cc djencext.cc
  djenclol.cc djencode.cc djencpro.cc djencsps.cc djencsv1.cc
  djrplol.cc djrploss.cc djutils.cc)

IF(DCMTK_WITH_OPENJPEG2)
  LIST(APPEND DCMJPEG_SOURCES
    openjp2k.cc djdec2k djdec2k.cc djdijp2k.cc djeijg2k.cc djenc2k.cc)
ENDIF(DCMTK_WITH_OPENJPEG2)

DCMTK_ADD_LIBRARY(dcmjpeg ${DCMJPEG_SOURCES})

DCMTK_TARGET_LINK_MODULES(dcmjpeg
  ofstd oflog dcmdata dcmimgle dcmimage ijg8 ijg12 ijg16)

IF(DCMTK_WITH_OPENJPEG2)
  TARGET_LINK_LIBRARIES(dcmjpeg ${LIBOPENJPEG2_LIBRARY})
ENDIF(DCMTK_WITH_OPENJPEG2)
