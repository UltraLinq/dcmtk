# declare additional include directories
INCLUDE_DIRECTORIES(${dcmjpeg_SOURCE_DIR}/include ${ofstd_SOURCE_DIR}/include ${oflog_SOURCE_DIR}/include ${dcmdata_SOURCE_DIR}/include ${dcmimgle_SOURCE_DIR}/include ${dcmimage_SOURCE_DIR}/include ${ZLIB_INCDIR} ${LIBTIFF_INCDIR} ${LIBPNG_INCDIR})

# declare executables
FOREACH(PROGRAM dcmcjpeg dcmdjpeg dcmj2pnm dcmmkdir)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dcmcjpeg dcmdjpeg dcmj2pnm dcmmkdir)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmjpeg ijg8 ijg12 ijg16 dcmimage dcmimgle dcmdata oflog ofstd)
ENDFOREACH(PROGRAM)

IF(BUILD_WINDOWS_CONSOLE_PROGRAMS)
  DCMTK_ADD_EXECUTABLE(dcmj2pnm-console dcmj2pnm)
  DCMTK_TARGET_LINK_MODULES(dcmj2pnm-console dcmjpeg ijg8 ijg12 ijg16 dcmimage dcmimgle dcmdata oflog ofstd)
  DCMTK_TARGET_LINK_LIBRARIES(dcmj2pnm ${WINDOWS_SUBSYSTEM_LINKER_FLAGS})
ENDIF(BUILD_WINDOWS_CONSOLE_PROGRAMS)

