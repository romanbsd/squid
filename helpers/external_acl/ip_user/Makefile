# $Id: Makefile,v 1.1 2002/06/24 06:08:39 hno Exp $
CC=gcc
CFLAGS=-Wall
LIBS=
# For Solaris
# LIBS=-lnsl -lsocket

ip_user_check: ip_user.h dict.o match.c main.c
				$(CC) $(CFLAGS) -o ip_user_check dict.o match.c main.c $(LIBS)

dict.o: ip_user.h dict.c
		$(CC) $(CFLAGS) -c dict.c

clean:
		rm -f dict.o ip_user_check

