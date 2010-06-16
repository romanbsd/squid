#
#  Makefile for the Squid Object Cache server
#
#  $Id: Makefile.am,v 1.5.6.1 2010/03/14 20:25:54 hno Exp $
#
#

libexec_PROGRAMS = yp_auth
yp_auth_SOURCES	= yp_auth.c nis_support.h nis_support.c
LDADD	= -L$(top_builddir)/lib -lmiscutil $(CRYPTLIB) $(NIS_LIBS) $(XTRA_LIBS)
INCLUDES	= -I$(top_srcdir)/include
