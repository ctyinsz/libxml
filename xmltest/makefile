SHLIBDIR=-L${HOME}/lib
BINDIR=.
CC=cc
INCLUDE=-I./
VPATH=./

LINKLIB= -lxml
LINKLIBS= -lsxml


all:xmltest sxmltest

xmltest:xmltest.c
	${CC}  -g ./xmltest.c -o  xmltest  ${DEBUG} ${CCFLAG} ${LDFLAG} ${LIBINCL} ${SHLIBDIR} ${LINKLIB}
sxmltest:xmltest.c
	${CC}  -g ./xmltest.c -o  sxmltest  ${DEBUG} ${CCFLAG} ${LDFLAG} ${LIBINCL} ${SHLIBDIR} ${LINKLIBS}

clean:
	rm ${BINDIR}/xmltest
	rm ${BINDIR}/sxmltest	