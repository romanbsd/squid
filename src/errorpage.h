

/* $Id: errorpage.h,v 1.7 1996/04/05 23:21:11 wessels Exp $ */

extern void cached_error_entry _PARAMS((StoreEntry *, int, char *));
extern char *cached_error_url _PARAMS((char *, int, int, char *, int, char *));
extern char *cached_error_request _PARAMS((char *, int, char *, int));
extern void errorInitialize _PARAMS((void));

extern char *tmp_error_buf;
