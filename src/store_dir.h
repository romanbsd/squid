/*
 * $Id: store_dir.h,v 1.5 1997/04/25 20:15:36 wessels Exp $
 *
 * AUTHOR: Duane Wessels
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

#ifndef _STORE_DIR_H_
#define _STORE_DIR_H_

#define MAX_FILES_PER_DIR (1<<20)

typedef struct {
    char *path;
    int l1;
    int l2;
    int cur_size;
    int max_size;
    int read_only;
    int suggest;
    fileMap *map;
    int swaplog_fd;
} SwapDir;

extern char *storeSwapFullPath _PARAMS((int, char *));
extern char *storeSwapSubSubDir _PARAMS((int, char *));
extern int storeAddSwapDisk _PARAMS((const char *,
	int size,
	int l1,
	int l2,
	int read_only));
extern int storeVerifySwapDirs _PARAMS((void));
extern void storeCreateSwapSubDirs _PARAMS((int));
extern const char *storeSwapPath _PARAMS((int));
extern int storeDirMapBitTest _PARAMS((int fn));
extern void storeDirMapBitSet _PARAMS((int fn));
extern void storeDirMapBitReset _PARAMS((int fn));
extern int storeDirMapAllocate _PARAMS((void));
extern char *storeSwapDir _PARAMS((int));
extern FILE * storeDirOpenTmpSwapLog _PARAMS((int dirn, int *clean_flag));
extern void storeDirCloseTmpSwapLog _PARAMS((int dirn));
extern void storeDirOpenSwapLogs _PARAMS((void));
extern void storeDirCloseSwapLogs _PARAMS((void));
extern char *storeDirSwapLogFile _PARAMS((int, const char *));
extern void storeDirSwapLog _PARAMS((const StoreEntry *));
extern int storeDirNumber _PARAMS((int fileno));
extern void storeDirUpdateSwapSize _PARAMS((int fn, size_t size));

extern int ncache_dirs;

#endif
