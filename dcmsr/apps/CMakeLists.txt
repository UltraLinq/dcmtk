# declare executables
FOREACH(PROGRAM dsr2html dsr2xml dsrdump xml2dsr)
  DCMTK_ADD_EXECUTABLE(${PROGRAM} ${PROGRAM})
ENDFOREACH(PROGRAM)

# make sure executables are linked to the corresponding libraries
FOREACH(PROGRAM dsr2html dsr2xml dsrdump xml2dsr)
  DCMTK_TARGET_LINK_MODULES(${PROGRAM} dcmsr dcmimage dcmimgle dcmdata oflog ofstd)
ENDFOREACH(PROGRAM)

IF(BUILD_WINDOWS_CONSOLE_PROGRAMS)
  DCMTK_ADD_EXECUTABLE(dsr2xml-console dsr2xml)
  DCMTK_TARGET_LINK_LIBRARIES(dsr2xml ${WINDOWS_SUBSYSTEM_LINKER_FLAGS})
  DCMTK_TARGET_LINK_MODULES(dsr2xml-console dcmsr dcmimage dcmimgle dcmdata oflog ofstd)
ENDIF(BUILD_WINDOWS_CONSOLE_PROGRAMS)
