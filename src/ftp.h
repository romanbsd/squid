
/* $Id: ftp.h,v 1.5 1996/04/04 01:30:43 wessels Exp $ */

extern int ftpStart _PARAMS((int unusedfd, char *url, StoreEntry * entry));
extern int ftpInitialize _PARAMS((void));
extern int ftpCachable _PARAMS((char *));
