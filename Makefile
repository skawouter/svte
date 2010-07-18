# svte: simple virtual terminal emulator: minimal, tabbed, VTE-based terminal
# Copyright 2010 svte contributors.
#
# This file is part of svte.
#
# svte is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.


include config.mk

SRC = svte.c
OBJ = ${SRC:.c=.o}

all: options svte

options:
	@echo svte build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.mk


svte: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ svte.o ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f svte ${OBJ} mt-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p svte-${VERSION}
	@cp -R Makefile config.mk ${SRC} svte-${VERSION}
	@tar -cf svte-${VERSION}.tar mt-${VERSION}
	@gzip svte-${VERSION}.tar
	@rm -rf svte-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f svte ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/svte

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/svte


.PHONY: all options clean dist install uninstall
