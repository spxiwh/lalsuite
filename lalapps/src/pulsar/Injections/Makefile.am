BUILT_SOURCES =
include $(top_srcdir)/gnuscripts/lalapps.am

bin_PROGRAMS = 	lalapps_Makefakedata_v4 \
		lalapps_Makefakedata_v5 \
		lalapps_compareSFTs \
		lalapps_compareTS \
		lalapps_dumpSFT

EXTRA_PROGRAMS = eah_Makefakedata_v4 eah_Makefakedata_v5

if LALFRAME
EXTRA_PROGRAMS += lalapps_sw_inj_frames
endif

lalapps_Makefakedata_v4_SOURCES = makefakedata_v4.c

lalapps_Makefakedata_v5_SOURCES = makefakedata_v5.c

# force linking in C++ mode for E@H
eah_Makefakedata_v4_SOURCES = $(lalapps_Makefakedata_v4_SOURCES) ../EinsteinAtHome/win_lib.cpp

eah_Makefakedata_v5_SOURCES = $(lalapps_Makefakedata_v5_SOURCES) ../EinsteinAtHome/win_lib.cpp

lalapps_compareSFTs_SOURCES = compareSFTs.c

lalapps_compareTS_SOURCES = compareTS.c

lalapps_dumpSFT_SOURCES = dumpSFT.c

## ----- extra-programs
lalapps_sw_inj_frames_SOURCES = sw_inj_frames.c

EXTRA_DIST = testMFDv4.sh testMFDv5.sh testT8_1800
TESTS = testMFDv4.sh testMFDv5.sh

TESTS_ENVIRONMENT = \
	LAL_DEBUG_LEVEL=msglvl1,memdbg; export LAL_DEBUG_LEVEL; \
	LAL_DATA_PATH="$(LAL_DATA_PATH)"; export LAL_DATA_PATH;
