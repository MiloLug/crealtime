# crealtime - c real time
# See LICENSE file for copyright and license details.

include config.mk

SRC = main.c
OBJ = ${SRC:.c=.o}
BIN_NAME = crealtime

all: options ${BIN_NAME}

options:
	@echo ${BIN_NAME} build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: config.h config.mk

config.h:
	cp config.def.h $@

${BIN_NAME}: ${OBJ}
	${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	rm -f ${BIN_NAME} ${OBJ} ${BIN_NAME}-${VERSION}.tar.gz

dist: clean
	mkdir -p ${BIN_NAME}-${VERSION}
	cp -R LICENSE Makefile README config.def.h config.mk\
		  ${SRC} ${BIN_NAME}-${VERSION}
	tar -cf ${BIN_NAME}-${VERSION}.tar ${BIN_NAME}-${VERSION}
	gzip ${BIN_NAME}-${VERSION}.tar
	rm -rf ${BIN_NAME}-${VERSION}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ${BIN_NAME} ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/${BIN_NAME}

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/${BIN_NAME}

.PHONY: all options clean dist install uninstall
