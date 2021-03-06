BUILT_SOURCES =
include $(top_srcdir)/gnuscripts/lalapps.am

AM_CPPFLAGS = -I$(top_srcdir)/src/pulsar/Fstatistic @AM_CPPFLAGS@

## Compiling with OpenCL on Linux
# LDFLAGS = -lOpenCL
#
## Compiling with OpenCL on Mac
# LDFLAGS = -framework OpenCL

# how to compile CUDA files (.cu)
NVCC ?= nvcc
.cu.o:
	$(NVCC) $(CPPFLAGS) -c $< -o $@
# allow to specify a different CUDA runtime library (e.g. for Windows when cross-compiling)
CUDART ?= -lcudart

bin_PROGRAMS = lalapps_HoughValidateAM lalapps_HoughMismatch lalapps_HierarchicalSearch

EXTRA_PROGRAMS = lalapps_HierarchicalSearch_sse lalapps_HierarchicalSearch_sse2 \
		 eah_HierarchicalSearch

# lalapps_HierarchicalSearch doesn't condor compile
#if CONDOR_ENABLED
#EXTRA_PROGRAMS += lalapps_HierarchicalSearch
#else
#bin_PROGRAMS += lalapps_HierarchicalSearch
#endif

EAHsrc = $(top_srcdir)/src/pulsar/EinsteinAtHome
FDSsrc = ../../Fstatistic

lalapps_HoughValidateAM_SOURCES = HoughValidateAM.c ../src/MCInjectHoughS2.h \
	                  ../src/SFTbin.h ../src/SFTbin.c \
			  ../src/DriveHoughColor.h ../src/PeakSelect.h \
			  ../src/PeakSelect.c

lalapps_HoughMismatch_SOURCES = HoughMismatch.c ../src/MCInjectHoughS2.h \
			../src/SFTbin.h ../src/SFTbin.c \
			../src/DriveHoughColor.h ../src/PeakSelect.h \
			../src/PeakSelect.c

HierarchicalSearchSources = HierarchicalSearch.c HierarchicalSearch.h \
			  StackSlideFstat.c StackSlideFstat.h \
			  HoughFstatToplist.c HoughFstatToplist.h \
			  $(FDSsrc)/HeapToplist.c $(FDSsrc)/HeapToplist.h

lalapps_HierarchicalSearch_SOURCES = $(HierarchicalSearchSources)

HSLocalSources = $(EAHsrc)/LocalComputeFstatHoughMap.c
EAHSources = $(HSLocalSources) $(EAHsrc)/hs_boinc_extras.c $(EAHsrc)/hs_boinc_options.cpp $(EAHsrc)/win_lib.cpp

## optimized HierarchicalSearch targets

# compilation of SSE targets requires a valid CPU architecture being set in AM_CFLAGS
# (e.g. configure with CFLAGS=-march=pentium-m),
lalapps_HierarchicalSearch_sse_SOURCES = $(HierarchicalSearchSources) $(HSLocalSources)
lalapps_HierarchicalSearch_sse_CPPFLAGS = $(AM_CPPFLAGS) -DCOMPUTEFSTATHOUGHMAP=LocalComputeFstatHoughMap
lalapps_HierarchicalSearch_sse_CFLAGS = $(AM_CFLAGS) -msse -mfpmath=sse
lalapps_HierarchicalSearch_sse2_SOURCES = $(HierarchicalSearchSources) $(HSLocalSources)
lalapps_HierarchicalSearch_sse2_CPPFLAGS = $(AM_CPPFLAGS) -DCOMPUTEFSTATHOUGHMAP=LocalComputeFstatHoughMap
lalapps_HierarchicalSearch_sse2_CFLAGS = $(AM_CFLAGS) -msse -msse2 -mfpmath=sse

# Einstein@home targets
eah_HierarchicalSearch_SOURCES = $(HierarchicalSearchSources) $(EAHSources)
eah_HierarchicalSearch_CPPFLAGS = $(AM_CPPFLAGS) -DEAH_BOINC -DHS_OPTIMIZATION
# this shouldn't harm - it just prefers libraries in the build directory over e.g. system ones
eah_HierarchicalSearch_LDFLAGS = -L. $(AM_LDFLAGS)
# RELEASE_DEPS contains additional dependencies of the E@H build
# items in there should have rules listed below to create them
# the idea is to increase compatibility of E@H applications by linking these libs statically
eah_HierarchicalSearch_DEPENDENCIES = $(RELEASE_DEPS)
eah_HierarchicalSearch_LDADD = $(LDADD) $(RELEASE_LDADD)

libstdc++.a:
	cp -f `$(CC) -print-file-name=$@` $@

exchndl.o: $(EAHsrc)/exchndl.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

erp_execinfo_plus.o: $(EAHsrc)/erp_execinfo_plus.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

TESTS_ENVIRONMENT = \
	LAL_DEBUG_LEVEL=msglvl1,memdbg; export LAL_DEBUG_LEVEL; \
	LAL_DATA_PATH="$(LAL_DATA_PATH)"; export LAL_DATA_PATH;
