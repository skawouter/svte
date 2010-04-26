# multi-terminal - a simple gtk terminal

include config.mk

SRC = mt.c
OBJ = ${SRC:.c=.o}

all: options mt

options:
	@echo mt build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.mk


mt: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ mt.o ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f mt ${OBJ} mt-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p mt-${VERSION}
	@cp -R Makefile config.mk ${SRC} mt-${VERSION}
	@tar -cf mt-${VERSION}.tar mt-${VERSION}
	@gzip mt-${VERSION}.tar
	@rm -rf mt-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f mt ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/mt

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/mt


.PHONY: all options clean dist install uninstall
