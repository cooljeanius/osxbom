## automake Makefile for lsbom

NAME=lsbom

CXX=@CXX@

bin_PROGRAMS = $(NAME)

lsbom_SOURCES = lsbom.cpp bom.h
lsbom_CPPFLAGS = -I.
lsbom_CFLAGS = -Wall -Wextra @MAINT@ -Werror
lsbom_CXXFLAGS = -Wall -Wextra -Wc++11-compat -Wuseless-cast -Wformat=2 -Wundef -Wlogical-op -Winvalid-pch @MAINT@ -Waggregate-return -Wshadow -Wconversion -Wsign-conversion -Wmissing-declarations -Wzero-as-null-pointer-constant -pedantic -std=gnu++98

noinst_HEADERS = bom.h

all: $(NAME)
.PHONY: all

$(NAME)_old: $(NAME).cpp bom.h
	$(CXX) -o $@ $< -I. -Wall -Wextra @MAINT@ -Werror
.PHONY: $(NAME)_old

clean-local:
	@RM_PROG@ -f $(NAME) $(NAME).o
.PHONY: clean-local

DISTCLEANFILES = *~ autoscan.log .DS_Store

distclean-local: clean-local
	@RM_PROG@ -rfv autom4te.cache || rmdir autom4te.cache
.PHONY: distclean-local

EXTRA_DIST = test.bom Portfile TODO Makefile_orig_hardcoded

check-local: ./$(NAME) ${srcdir}/test.bom
	./$(NAME) ${srcdir}/test.bom
.PHONY: check-local
