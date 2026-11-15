.POSIX:
.SUFFIXES: .3 .3.md

# project version
VERSION = 0.2.0
# date that documentation should report
DATE = $$(date +%Y-%m-%d)

# install prefix
PREFIX = /usr/local
# directory to install libraries to
LIBDIR = $(PREFIX)/lib
# directory to install header files to
INCLUDEDIR = $(PREFIX)/include
# directory to install manpages to
MANDIR = $(PREFIX)/share/man

# fake root to prepend to the prefix for staged installs; not included in
# any generated files, only used for creating directories and copying files
DESTDIR =

# c compiler
CC = c99
# archive tool
AR = ar
# md2manc binary to use for generating documentation
MD2MANC = md2manc

# c compilation flags; override without -W* if compiler does not support warning
# flags
CFLAGS = -g -O0 -Wall -Wextra -Wpedantic -Wvla
# archive creation flags
ARFLAGS = rcs
# c preprocessing flags; if overriding this, make sure to include the original
# flags or the build may break
CPPFLAGS = -D_POSIX_C_SOURCE=200809L
# c linkage flags
LDFLAGS =
# library dependencies to link
LDLIBS = -lbacktrace

# object list
OBJS = raisetrace_posix.o raisetrace_win32.o
# manpage list; override to empty value to skip docs
DOCS = RAISETRACE_CONTINUE.3 RAISETRACE_DEFAULT.3 raisetrace.3 \
       raisetrace_callback.3 raisetrace_faults.3 raisetrace_unhandled.3

all: compile_flags.txt libraisetrace.a raisetrace.pc $(DOCS)

install:
	mkdir -p $(DESTDIR)$(INCLUDEDIR)
	cp -f raisetrace.h $(DESTDIR)$(INCLUDEDIR)

	mkdir -p $(DESTDIR)$(LIBDIR)/pkgconfig
	cp -f libraisetrace.a $(DESTDIR)$(LIBDIR)
	cp -f raisetrace.pc $(DESTDIR)$(LIBDIR)/pkgconfig

	mkdir -p $(DESTDIR)$(MANDIR)/man3
	[ -n "$(DOCS)" ] && cp -f $(DOCS) $(DESTDIR)$(MANDIR)/man3

uninstall:
	-cd $(DESTDIR)$(INCLUDEDIR) && rm -f raisetrace.h
	-cd $(DESTDIR)$(LIBDIR) && rm -f libraisetrace.a
	-cd $(DESTDIR)$(LIBDIR)/pkgconfig && rm -f raisetrace.pc
	-[ -n "$(DOCS)" ] && cd $(DESTDIR)$(MANDIR)/man3 && rm -f $(DOCS)

clean:
	-rm -f compile_flags.txt libraisetrace.a raisetrace.pc $(OBJS) $(DOCS)

format:
	sh check_format.sh

.c.o:
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<
.3.md.3:
	$(MD2MANC) -d "$(DATE)" -S "raisetrace $(VERSION)" $< > $@

$(OBJS): raisetrace.h

compile_flags.txt:
	echo '-xc -std=c99 $(CFLAGS) $(CPPFLAGS)' | tr " " "\n" > $@
	echo '-Wno-empty-translation-unit' >> $@

libraisetrace.a: $(OBJS)
	$(AR) $(ARFLAGS) $@ $?
raisetrace.pc:
	echo prefix=$(PREFIX) > $@
	[ $(PREFIX)/include = $(INCLUDEDIR) ] && \
		echo includedir='$${prefix}'/include >> $@ || \
		echo includedir=$(INCLUDEDIR) >> $@
	[ $(PREFIX)/lib = $(LIBDIR) ] && \
		echo libdir='$${prefix}'/libdir >> $@ || \
		echo libdir=$(LIBDIR) >> $@
	echo >> $@
	echo Name: raisetrace >> $@
	echo Version: $(VERSION) >> $@
	echo Description: C library for generating stack traces on signal/exception >> $@
	echo URL: https://github.com/deimonn/raisetrace >> $@
	echo Cflags: -I'$${includedir}' >> $@
	echo Libs: -L'$${libdir}' $(LDFLAGS) -lraisetrace $(LDLIBS) >> $@

RAISETRACE_CONTINUE.3 RAISETRACE_DEFAULT.3:
	echo .so man3/raisetrace_callback.3 > $@
raisetrace_unhandled.3:
	echo .so man3/raisetrace_faults.3 > $@
