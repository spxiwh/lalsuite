BUILT_SOURCES =
MOSTLYCLEANFILES =
include $(top_srcdir)/gnuscripts/lalsuite_header_links.am
include $(top_srcdir)/gnuscripts/lalsuite_vcs_info.am

if LALXML
XMLHDR = LALInferenceXML.h
XMLSRC = LALInferenceXML.c
endif

if HAVE_CHEALPIX
BAYESTARHDR = bayestar_sky_map.h
BAYESTARSRC = bayestar_sky_map.c
endif

ANALYTICFILES = CorrelationMatrix.csv unimodalMeans.csv bimodalMeans.csv

pkgdata_DATA = $(ANALYTICFILES)

pkginclude_HEADERS = \
	LALInference.h \
	LALInferenceConfig.h \
	LALInferenceLikelihood.h \
	LALInferenceMultibanding.h \
	LALInferenceNestedSampler.h \
	LALInferencePrior.h \
	LALInferenceReadBurstData.h \
	LALInferenceReadData.h \
	LALInferenceTemplate.h \
	LALInferenceProposal.h \
	LALInferenceClusteredKDE.h \
	LALInferenceKDE.h \
	LALInferenceVCSInfo.h \
	LALInferenceInit.h \
	LALInferenceVCSInfoHeader.h \
	LALInferenceRemoveLines.h \
	LALInferenceCalibrationErrors.h \
	LALInferenceGenerateROQ.h \
	LALInferenceBurstRoutines.h \
	$(BAYESTARHDR) \
	$(XMLHDR)

lib_LTLIBRARIES = liblalinference.la
liblalinference_la_SOURCES = \
	LALInference.c \
	LALInferenceLikelihood.c \
	LALInferenceAnalyticLikelihood.c \
	LALInferenceMultibanding.c \
	LALInferenceNestedSampler.c \
	LALInferencePrior.c \
	LALInferenceReadBurstData.c \
	LALInferenceReadData.c \
	LALInferenceTemplate.c \
	LALInferenceProposal.c \
	LALInferenceClusteredKDE.c \
	LALInferenceKDE.c \
	LALInferenceInitBurst.c \
	LALInferenceInitCBC.c \
	LALInferenceVCSInfo.c \
	LALInferenceRemoveLines.c \
	LALInferenceCalibrationErrors.c \
	LALInferenceGenerateROQ.c \
	LALInferenceBurstRoutines.c \
	DetectorFixedSkyCoords.c \
	logaddexp.h \
	$(BAYESTARSRC) \
	$(XMLSRC)
nodist_liblalinference_la_SOURCES = LALInferenceBuildInfo.c

liblalinference_la_LDFLAGS = $(AM_LDFLAGS) -version-info $(LIBVERSION)

EXTRA_DIST = \
	LALInferenceConfig.h.in \
	LALInferenceXML.c \
	LALInferenceXML.h \
	$(ANALYTICFILES) \
	config.h.in

DISTCLEANFILES = config.h

vcs_info_sources = LALInferenceVCSInfoHeader.h
build_info_source = LALInferenceBuildInfo.c
$(build_info_source): $(liblalinference_la_DEPENDENCIES)
