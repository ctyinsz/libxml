#include $(INCLMAKE)
#include ${HOME}/src/etc/makefile.${GAPSPLATFORM}
MYLDFLAG= -brlt
CMSTMP=${APPHOME}/tmp
CCFLAG=-D__ORACLE__
SHLIBDIR=${HOME}/shlib
LIBDIR=${HOME}/lib
RELEASE_VERSION=2.0.0
CC=gcc ${OS_CCFLAG} -m32
VPATH=${CMSTMP}:.libs:.:

.SUFFIXES:
.SUFFIXES:.c .lo
.c.lo:
	libtool --mode=compile --tag=CC ${CC} -g -O -o ${CMSTMP}/$*.lo  -c $<

all:libxml.la 
libxml.la:xmle.lo xml.lo xmlnode.lo trace.lo
	libtool --mode=link --tag=CC ${CC} ${OS_LDFLAG} ${MYLDFLAG} -g  -O -o ${CMSTMP}/$(@:.so=.la) $? -rpath ${LIBDIR}  -release ${RELEASE_VERSION} -export-dynamic -no-undefined -module

install:
	libtool --mode=install install -c ${CMSTMP}/*.la ${LIBDIR}
	libtool --mode=finish  ${LIBDIR}

clean:
	@rm -f lib*.a *~ *core ${CMSTMP}/*.lo ${CMSTMP}/*.o ${CMSTMP}/*.la ${CMSTMP}/.libs/*
	@rm -rf ./.libs
