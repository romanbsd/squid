

/* $Id: errorpage.h,v 1.6 1996/04/04 17:46:39 wessels Exp $ */

void cached_error_entry _PARAMS((StoreEntry *, int, char *));
char *cached_error_url _PARAMS((char *, int, int, char *, int, char *));
char *cached_error_request _PARAMS((char *, int, char *, int));

extern char tmp_error_buf[];
