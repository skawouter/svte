# surf - simple browser
# See LICENSE file for copyright and license details.

include config.mk

SRC = sb.c
OBJ = ${SRC:.c=.o}

all: options sb

options:
	@echo sb build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.mk


sb: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ sb.o ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f sb ${OBJ} sb-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p sb-${VERSION}
	@cp -R Makefile config.mk ${SRC} sb-${VERSION}
	@tar -cf sb-${VERSION}.tar sb-${VERSION}
	@gzip sb-${VERSION}.tar
	@rm -rf sb-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f sb ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/sb

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/sb


.PHONY: all options clean dist install uninstall
