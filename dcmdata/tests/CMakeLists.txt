# declare executables
DCMTK_ADD_EXECUTABLE(dcmdata_tests tests tpread ti2dbmp tchval tpath tvrdatim telemlen tparser tdict tvrds tvrfd tvrui tstrval tspchrs tvrpn tparent tfilter tvrcomp)

# make sure executables are linked to the corresponding libraries
DCMTK_TARGET_LINK_MODULES(dcmdata_tests i2d dcmdata oflog ofstd)

# This macro parses tests.cc and registers all tests
DCMTK_ADD_TESTS(dcmdata)
