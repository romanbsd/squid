

/* $Id: errorpage.h,v 1.8 1996/04/11 04:47:21 wessels Exp $ */

extern void cached_error_entry _PARAMS((StoreEntry *, int, char *));
extern char *cached_error_url _PARAMS((char *, int, int, char *, int, char *));
extern char *cached_error_request _PARAMS((char *, int, char *, int));
extern void errorInitialize _PARAMS((void));
extern char *access_denied_msg _PARAMS((int, int, char *, char *));

extern char *tmp_error_buf;
