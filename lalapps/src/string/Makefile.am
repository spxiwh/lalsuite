BUILT_SOURCES =
MOSTLYCLEANFILES =
EXTRA_DIST =
include $(top_srcdir)/gnuscripts/lalsuite_python.am
include $(top_srcdir)/gnuscripts/lalapps.am

bin_PROGRAMS = lalapps_StringSearch lalapps_StringAddFrame 
lalapps_StringSearch_SOURCES = StringSearch.c
lalapps_StringAddFrame_SOURCES = AddFrame.c

if HAVE_PYTHON
pybin_scripts = \
	lalapps_cosmicstring_pipe \
	lalapps_string_cs_gamma
pkgpython_PYTHON = cosmicstring.py
endif

#pkgdata_DATA = S3_H1_StringDag.ini S3_StringDag.ini S4_StringDag.ini

EXTRA_DIST += cosmicstring.py $(DATA)
