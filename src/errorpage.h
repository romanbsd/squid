

/* $Id: errorpage.h,v 1.5 1996/04/04 01:30:42 wessels Exp $ */

void cached_error_entry _PARAMS((StoreEntry *, int, char *));
char *cached_error_url _PARAMS((char *, int, int, char *, char *));
char *cached_error_request _PARAMS((char *, int, char *));

extern char tmp_error_buf[];
