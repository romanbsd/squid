/*  $Id: icp_lib.h,v 1.2 1996/03/27 01:46:10 wessels Exp $ */

#ifndef ICP_LIB_H
#define ICP_LIB_H

typedef struct obj {
    icp_common_t header;
    char *url;
    unsigned long ttl;
    unsigned long timestamp;
    unsigned long object_size;
    unsigned long buf_len;
    unsigned long offset;
    char *data;
} icp_object;

#endif
