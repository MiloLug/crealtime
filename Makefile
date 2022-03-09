# crealtime - c real time
# See LICENSE file for copyright and license details.

include config.mk

SRC = main.c
OBJ = ${SRC:.c=.o}

all: options crealtime

options:
	@echo crealtime build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: config.h config.mk

config.h:
	cp config.def.h $@

crealtime: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f crealtime ${OBJ} crealtime-${VERSION}.tar.gz

dist: clean
	mkdir -p crealtime-${VERSION}
	cp -R LICENSE Makefile README config.def.h config.mk\
		  ${SRC} crealtime-${VERSION}
	tar -cf crealtime-${VERSION}.tar crealtime-${VERSION}
	gzip crealtime-${VERSION}.tar
	rm -rf crealtime-${VERSION}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f crealtime ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/crealtime

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/crealtime

.PHONY: all options clean dist install uninstall
