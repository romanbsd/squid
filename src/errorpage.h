
/* $Id: errorpage.h,v 1.4 1996/03/30 00:09:03 wessels Exp $ */

void cached_error_entry _PARAMS((StoreEntry *, int, char *));
char *cached_error_url _PARAMS((char *, int, char *));

extern char tmp_error_buf[];
