/*  $Id: url.h,v 1.3 1996/04/10 03:54:00 wessels Exp $ */

#ifndef _URL_HEADER_
#define _URL_HEADER_

#define MAX_URL  (ICP_MAX_URL)

extern char *url_convert_hex _PARAMS((char *org_url, int allocate));
extern char *url_escape _PARAMS((char *url));

#endif /* _URL_HEADER_ */
