# Makefile for SQUID Cache
#
# $Id: Makefile,v 1.2 1996/03/25 19:43:40 wessels Exp $
#

prefix=/usr/local/squid
exec_prefix=${prefix}

all: makefile doall

doall:
	@${MAKE} -f makefile all

squid: makefile dosquid

dosquid:
	@${MAKE} -f makefile squid

.DEFAULT:
	@if test \! -f makefile; then ${MAKE} makefile; fi
	@${MAKE} -f makefile $@

makefile: makefile.in Makefile
	@echo Running configure script to generate makefile
	@echo
	./configure --prefix=${prefix} --exec_prefix=${exec_prefix}
