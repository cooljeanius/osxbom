## automake Makefile for lsbom

NAME=lsbom

CXX=@CXX@

bin_PROGRAMS = $(NAME)

lsbom_SOURCES = lsbom.cpp bom.h
lsbom_CPPFLAGS = -I.
lsbom_CFLAGS = -Wall -Wextra @MAINT@ -Werror
lsbom_CXXFLAGS = -std=gnu++98 -Wall -Wextra -Wformat=2 -Wundef -Winvalid-pch -Wshadow -Wswitch -Wmissing-noreturn @MAINT@ -Wlogical-op -Wc++11-compat -Wuseless-cast -Waggregate-return -Wconversion -Wsign-conversion -Wmissing-declarations -Wswitch-default -Wzero-as-null-pointer-constant -pedantic
##FIXME: check which warning flags are supported in the configure script

noinst_HEADERS = bom.h

man_MANS = osxbom.1 lsbom.8

dist_doc_DATA = README

all: $(NAME)
.PHONY: all

$(NAME)_old: $(NAME).cpp bom.h
	$(CXX) -o $@ $< -I. -Wall -Wextra @MAINT@ -Werror
.PHONY: $(NAME)_old

clean-local:
	@RM_PROG@ -f $(NAME) $(NAME).o
.PHONY: clean-local

DISTCLEANFILES = *~ autoscan.log gdb.log .DS_Store out.txt @MAINT@ lsbom.8

distclean-local: clean-local
	@RM_PROG@ -fv @PACKAGE_TARNAME@-@PACKAGE_VERSION@.tar.gz
	@RM_PROG@ -fv @PACKAGE_TARNAME@-@PACKAGE_VERSION@.zip
	@RM_PROG@ -rfv autom4te.cache || rmdir autom4te.cache
	@RM_PROG@ -rfv *.dSYM || rmdir *.dSYM
	if test -d build; then \
	  @RM_PROG@ -rfv build || rmdir build; \
	elif test -d ${srcdir}/build; then \
	  @RM_PROG@ -rfv ${srcdir}/build || rmdir ${srcdir}/build; \
	fi
	(test -w @PACKAGE_TARNAME@-@PACKAGE_VERSION@ && (@RM_PROG@ -rfv @PACKAGE_TARNAME@-@PACKAGE_VERSION@ || rmdir @PACKAGE_TARNAME@-@PACKAGE_VERSION@)) || stat @PACKAGE_TARNAME@-*.*.* 2>&1 || echo "it can be difficult to distclean the results of distcheck"
.PHONY: distclean-local

EXTRA_DIST = test.bom Portfile TODO Makefile_orig_hardcoded
EXTRA_DIST += osxbom.xcodeproj/project.pbxproj osxbom.1 lsbom.8

check-local: ./$(NAME) ${srcdir}/test.bom
	./$(NAME) >/dev/null
	./$(NAME) ${srcdir}/test.bom | tee out.txt
.PHONY: check-local

EXTRA_DIST += en.lproj/lsbom.strings expected_results_for_test_bom.txt

en.lproj/lsbom.strings:
	if test -x /usr/bin/lsbom; then strings /usr/bin/lsbom | tee $@; fi

MAINTAINERCLEANFILES = configure.scan en.lproj/lsbom.strings lsbom.8

configure.scan:
	autoscan || touch configure.scan

lsbom.8:
	if test -e /usr/share/man/man8/lsbom.8; then \
	  cp -v /usr/share/man/man8/lsbom.8 .; \
	else \
	  touch $@; \
	fi

EXTRA_DIST += lsbom_disasm.s lsbom_disasm_annot.s

lsbom_disasm.s:
	if test -x /usr/bin/lsbom; then\
	  otool -tv /usr/bin/lsbom | tee $@; \
	else \
	  touch $@; \
	fi

lsbom_disasm_annot.s:
	if test -x /usr/bin/lsbom; then\
	  otool -tvV /usr/bin/lsbom | tee $@; \
	else \
	  touch $@; \
	fi

MAINTAINERCLEANFILES += lsbom_disasm.s lsbom_disasm_annot.s
