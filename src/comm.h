
/*
 * $Id: comm.h,v 1.50 1997/06/02 19:56:00 wessels Exp $
 *
 * AUTHOR: Harvest Derived
 *
 * SQUID Internet Object Cache  http://squid.nlanr.net/Squid/
 * --------------------------------------------------------
 *
 *  Squid is the result of efforts by numerous individuals from the
 *  Internet community.  Development is led by Duane Wessels of the
 *  National Laboratory for Applied Network Research and funded by
 *  the National Science Foundation.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *  
 */

/*
 * Copyright (c) 1994, 1995.  All rights reserved.
 *  
 *   The Harvest software was developed by the Internet Research Task
 *   Force Research Group on Resource Discovery (IRTF-RD):
 *  
 *         Mic Bowman of Transarc Corporation.
 *         Peter Danzig of the University of Southern California.
 *         Darren R. Hardy of the University of Colorado at Boulder.
 *         Udi Manber of the University of Arizona.
 *         Michael F. Schwartz of the University of Colorado at Boulder.
 *         Duane Wessels of the University of Colorado at Boulder.
 *  
 *   This copyright notice applies to software in the Harvest
 *   ``src/'' directory only.  Users should consult the individual
 *   copyright notices in the ``components/'' subdirectories for
 *   copyright information about other software bundled with the
 *   Harvest source code distribution.
 *  
 * TERMS OF USE
 *   
 *   The Harvest software may be used and re-distributed without
 *   charge, provided that the software origin and research team are
 *   cited in any use of the system.  Most commonly this is
 *   accomplished by including a link to the Harvest Home Page
 *   (http://harvest.cs.colorado.edu/) from the query page of any
 *   Broker you deploy, as well as in the query result pages.  These
 *   links are generated automatically by the standard Broker
 *   software distribution.
 *   
 *   The Harvest software is provided ``as is'', without express or
 *   implied warranty, and with no support nor obligation to assist
 *   in its use, correction, modification or enhancement.  We assume
 *   no liability with respect to the infringement of copyrights,
 *   trade secrets, or any patents, and are not responsible for
 *   consequential damages.  Proper use of the Harvest software is
 *   entirely the responsibility of the user.
 *  
 * DERIVATIVE WORKS
 *  
 *   Users may make derivative works from the Harvest software, subject 
 *   to the following constraints:
 *  
 *     - You must include the above copyright notice and these 
 *       accompanying paragraphs in all forms of derivative works, 
 *       and any documentation and other materials related to such 
 *       distribution and use acknowledge that the software was 
 *       developed at the above institutions.
 *  
 *     - You must notify IRTF-RD regarding your distribution of 
 *       the derivative work.
 *  
 *     - You must clearly notify users that your are distributing 
 *       a modified version and not the original Harvest software.
 *  
 *     - Any derivative product is also subject to these copyright 
 *       and use restrictions.
 *  
 *   Note that the Harvest software is NOT in the public domain.  We
 *   retain copyright, as specified above.
 *  
 * HISTORY OF FREE SOFTWARE STATUS
 *  
 *   Originally we required sites to license the software in cases
 *   where they were going to build commercial products/services
 *   around Harvest.  In June 1995 we changed this policy.  We now
 *   allow people to use the core Harvest software (the code found in
 *   the Harvest ``src/'' directory) for free.  We made this change
 *   in the interest of encouraging the widest possible deployment of
 *   the technology.  The Harvest software is really a reference
 *   implementation of a set of protocols and formats, some of which
 *   we intend to standardize.  We encourage commercial
 *   re-implementations of code complying to this set of standards.  
 */

#ifndef COMM_H
#define COMM_H

#define COMM_OK		  (0)
#define COMM_ERROR	 (-1)
#define COMM_NO_HANDLER	 (-2)
#define COMM_NOMESSAGE	 (-3)
#define COMM_TIMEOUT	 (-4)
#define COMM_SHUTDOWN	 (-5)
#define COMM_INPROGRESS  (-6)
#define COMM_ERR_CONNECT (-7)
#define COMM_ERR_DNS     (-8)

#define COMM_NONBLOCKING  (0x1)
#define COMM_NOCLOEXEC	  (0x8)

/* Select types. */
#define COMM_SELECT_READ   (0x1)
#define COMM_SELECT_WRITE  (0x2)

typedef void CWCB _PARAMS((int fd, char *, int size, int errflag, void *data));
typedef void CNCB _PARAMS((int fd, int status, void *data));
typedef void FREE _PARAMS((void *));

struct close_handler {
    PF *handler;
    void *data;
    struct close_handler *next;
};

extern int commSetNonBlocking _PARAMS((int fd));
extern void commSetCloseOnExec _PARAMS((int fd));
extern int comm_accept _PARAMS((int fd, struct sockaddr_in *, struct sockaddr_in *));
extern void comm_close _PARAMS((int fd));
extern void commConnectStart _PARAMS((int fd, const char *, u_short, CNCB *, void *));
extern int comm_connect_addr _PARAMS((int sock, const struct sockaddr_in *));
extern int comm_init _PARAMS((void));
extern int comm_listen _PARAMS((int sock));
extern int comm_open _PARAMS((int, int, struct in_addr, u_short port, int, const char *note));
extern u_short comm_local_port _PARAMS((int fd));
#if HAVE_POLL
extern int comm_poll _PARAMS((time_t));
#else
extern int comm_select _PARAMS((time_t));
#endif
extern void commSetSelect _PARAMS((int, unsigned int, PF *, void *, time_t));
extern void comm_add_close_handler _PARAMS((int fd, PF *, void *));
extern void comm_remove_close_handler _PARAMS((int fd, PF *, void *));
extern int comm_set_mcast_ttl _PARAMS((int, int));
extern void comm_join_mcast_groups _PARAMS((int, const ipcache_addrs *, void *));
extern int comm_udp_send _PARAMS((int fd, const char *host, u_short port, const char *buf, int len));
extern int comm_udp_sendto _PARAMS((int fd, const struct sockaddr_in *, int size, const char *buf, int len));
extern int fd_of_first_client _PARAMS((StoreEntry *));
extern void comm_set_stall _PARAMS((int, int));
extern void comm_write _PARAMS((int fd,
	char *buf,
	int size,
	CWCB * handler,
	void *handler_data,
	FREE *));
extern void commCallCloseHandlers _PARAMS((int fd));
extern void commCancelWriteHandler _PARAMS((int fd));
extern int commSetTimeout _PARAMS((int fd, int, PF *, void *));

extern int RESERVED_FD;

#endif /* COMM_H */
