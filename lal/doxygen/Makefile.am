TEST_EXTENSIONS = .sh
SH_LOG_COMPILER = $(SHELL)

if DOXYGEN

if HAVE_GIT_REPO
doxygen_include_gitid = true
endif # HAVE_GIT_REPO

doxygen_quiet   = $(doxygen_quiet_$(V))
doxygen_quiet_  = $(doxygen_quiet_$(AM_DEFAULT_VERBOSITY))
doxygen_quiet_0 = YES
doxygen_quiet_1 = NO

doxygen_redir   = $(doxygen_redir_$(V))
doxygen_redir_  = $(doxygen_redir_$(AM_DEFAULT_VERBOSITY))
doxygen_redir_0 = >/dev/null
doxygen_redir_1 =

html-local: Makefile $(CONFIG_CLEAN_FILES)
	$(AM_V_at)export doxygen_version="$(VERSION)"; \
	if test "x$(doxygen_include_gitid)" = xtrue; then \
		gitid=`cd $(abs_top_srcdir) && $(GIT) log -1 --pretty='format:%h'`; \
		if test "x$${gitid}" != x; then \
			unclean=`cd $(abs_top_srcdir) && ( $(GIT) diff --quiet HEAD || printf %s -UNCLEAN )`; \
			doxygen_version="$${doxygen_version}-$${gitid}$${unclean}"; \
		fi; \
	fi; \
	export doxygen_tagfiles; doxygen_tagfiles="$(DOXYGEN_TAGFILES)"; \
	export doxygen_quiet; doxygen_quiet="$(doxygen_quiet)"; \
	export doxygen_input; doxygen_input=; \
	for dir in src test python; do \
		test -d "$(abs_top_srcdir)/$${dir}" && doxygen_input="$${doxygen_input} $(abs_top_srcdir)/$${dir}"; \
	done; \
	for file in swig/SWIGCommon.i swig/SWIG$(PACKAGE_NAME)Alpha.i swig/SWIG$(PACKAGE_NAME)Omega.i; do \
		test -r "$(abs_top_srcdir)/$${file}" && doxygen_input="$${doxygen_input} $(abs_top_srcdir)/$${file}"; \
	done; \
	test -r "$(abs_srcdir)/exclude.list" || exit 1; \
	export doxygen_exclude; doxygen_exclude=`cat $(abs_srcdir)/exclude.list | tr '\n' ' '`; \
	echo '/* Generated by $(subdir)/Makefile */' > autogen.dox; \
	echo '/** \mainpage' >> autogen.dox; \
	test -r "$(abs_top_srcdir)/README" || exit 1; \
	cat $(abs_top_srcdir)/README >> autogen.dox; \
	echo '*/' >> autogen.dox; \
	echo '/** \page $(PACKAGE)_authors Author List' >> autogen.dox; \
	( cd $(abs_top_builddir) && $(MAKE) $(AM_MAKEFLAGS) AUTHORS ) || exit 1; \
	if test -f "$(abs_top_builddir)/AUTHORS"; then \
		doxygen_authors="$(abs_top_builddir)/AUTHORS"; \
	elif test -f "$(abs_top_srcdir)/AUTHORS"; then \
		doxygen_authors="$(abs_top_srcdir)/AUTHORS"; \
	else \
		echo "$(srcdir)/Makefile: Could not find AUTHORS" >&2; \
		exit 1; \
	fi; \
	$(SED) -e 's/^/- /' $${doxygen_authors} >> autogen.dox; \
	echo '*/' >> autogen.dox; \
	$(MKDIR_P) out/ || exit 1; \
	echo "=== Building Doxygen documentation for $(PACKAGE_NAME)..."; \
	$(DOXYGEN) doxygen.cfg $(doxygen_redir) 2>&1; \
	$(PYTHON) $(abs_srcdir)/check_tags.py $(abs_builddir)/out/$(PACKAGE).tag >> warnings.log || exit 1; \
	$(SED) $(DOXYGEN_WARNING_REGEX) warnings.log > errors.log; \
	cat errors.log >&2 || exit 1; \
	test `cat errors.log | wc -l` -eq 0 || exit 1; \
	rm -f doxygen_objdb_*.tmp doxygen_sqlite3.db; \
	echo "=== Doxygen documentation was built successfully!"; \
	echo "=== $(PACKAGE_NAME) main page is at file://$(abs_builddir)/out/index.html"

TESTS = test_doxygen.sh
test_doxygen.sh:
	$(AM_V_at)echo "cd $(abs_builddir) && make html" >$@

install-html-local: html-local
	$(AM_V_at)for dir in `cd out/ && find . -type d`; do \
		echo " $(MKDIR_P) '$(DESTDIR)$(htmldir)/$${dir}'"; \
		$(MKDIR_P) "$(DESTDIR)$(htmldir)/$${dir}" || exit 1; \
		$(PYTHON) $(abs_srcdir)/install_regex.py "$(DESTDIR)$(htmldir)/$${dir}" "$(DOXYGEN_INSTALL_DIRMAP)" > $@.regex || exit 1; \
		for file in `cd out/ && echo $${dir}/*`; do \
			if test -f "out/$${file}"; then \
				echo " $(INSTALL_DATA) out/$${file} '$(DESTDIR)$(htmldir)/$${file}'"; \
				$(SED) -n -f $@.regex "out/$${file}" > $@.tmp || exit 1; \
				$(INSTALL_DATA) $@.tmp "$(DESTDIR)$(htmldir)/$${file}" || exit 1; \
			fi; \
		done; \
	done; \
	rm -f $@.regex $@.tmp; \
	echo "=== Doxygen documentation was installed successfully!"; \
	echo "=== Installed $(PACKAGE_NAME) main page is at file://$(DESTDIR)$(htmldir)/index.html"

uninstall-local:
	-rm -rf "$(DESTDIR)$(htmldir)/"

clean-local:
	-rm -rf out/
	-rm -f autogen.dox errors.log warnings.log test_doxygen.sh

EXTRA_DIST = \
	check_tags.py \
	exclude.list \
	filter_i.sed \
	filter_py.py \
	install_regex.py \
	layout.xml \
	mathjax.js \
	src \
	stylesheet.css \
	toplevel.dox \
	$(END_OF_LIST)

endif # DOXYGEN
