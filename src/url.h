/*  $Id: url.h,v 1.2 1996/03/27 01:46:29 wessels Exp $ */

#ifndef _URL_HEADER_
#define _URL_HEADER_

#define MAX_URL  (ICP_MAX_URL)

extern char *url_convert_hex _PARAMS((char *org_url));
extern char *url_escape _PARAMS((char *url));

#endif /* _URL_HEADER_ */
