/*
 * snmp_client.c - a toolkit of common functions for an SNMP client.
 *
 */
/***********************************************************
	Copyright 1988, 1989 by Carnegie Mellon University

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of CMU not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
******************************************************************/
#include <sys/types.h>
#include <sys/param.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <errno.h>
#ifdef linux
#include <string.h>
#include <stdlib.h>
#endif

#include "asn1.h"
#include "snmp.h"
#include "snmp_impl.h"
#include "snmp_api.h"
#include "snmp_client.h"

#ifndef BSD4_3
#define BSD4_2
#endif

#if !defined(BSD4_3) && !defined(linux) && !defined(sun)

typedef long fd_mask;
#define NFDBITS	(sizeof(fd_mask) * NBBY)	/* bits per mask */

#define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)	bzero((char *)(p), sizeof(*(p)))
#endif


extern int errno;

static struct synch_state snmp_synch_state;

struct snmp_pdu *
snmp_pdu_create(command)
     int command;
{
    struct snmp_pdu *pdu;

    pdu = (struct snmp_pdu *) calloc(1, sizeof(struct snmp_pdu));
    bzero((char *) pdu, sizeof(struct snmp_pdu));
    pdu->command = command;
    pdu->errstat = SNMP_DEFAULT_ERRSTAT;
    pdu->errindex = SNMP_DEFAULT_ERRINDEX;
    pdu->address.sin_addr.s_addr = SNMP_DEFAULT_ADDRESS;
    pdu->enterprise = NULL;
    pdu->enterprise_length = 0;
    pdu->variables = NULL;
    return pdu;
}

/*
 * Add a null variable with the requested name to the end of the list of
 * variables for this pdu.
 */
void
snmp_add_null_var(struct snmp_pdu *pdu, oid * name, int name_length)
{
    struct variable_list *vars;

    if (pdu->variables == NULL) {
	pdu->variables = vars = (struct variable_list *) calloc(1, sizeof(struct variable_list));
    } else {
	for (vars = pdu->variables; vars->next_variable; vars = vars->next_variable);
	vars->next_variable = (struct variable_list *) calloc(1, sizeof(struct variable_list));
	vars = vars->next_variable;
    }

    vars->next_variable = NULL;
    vars->name = (oid *) calloc(1, name_length * sizeof(oid));
    bcopy((char *) name, (char *) vars->name, name_length * sizeof(oid));
    vars->name_length = name_length;
    vars->type = ASN_NULL;
    vars->val.string = NULL;
    vars->val_len = 0;
}

#if 1 /***/			/* safe var clone, to avoid weird variable pointers */
/*
 * Clone a variable, returns NULL in case of failure
 */
static struct variable_list *
clone_variable(var)
     struct variable_list *var;
{
    struct variable_list *newvar;
    if (!var)
	return NULL;
    newvar = (struct variable_list *) calloc(1, sizeof(struct variable_list));
    if (!newvar)
	return NULL;
    bcopy((char *) var, (char *) newvar, sizeof(struct variable_list));
    if (var->name != NULL) {
	newvar->name = (oid *) calloc(1, var->name_length * sizeof(oid));
	if (newvar->name == NULL) {	/* paranoia */
	    free(newvar);
	    return NULL;
	}
	bcopy((char *) var->name, (char *) newvar->name, var->name_length * sizeof(oid));
    }
    if (var->val.string != NULL) {
	newvar->val.string = (u_char *) calloc(1, var->val_len);
	if (newvar->val.string == NULL) {	/* paranoia */
	    if (newvar->name != NULL)
		free(newvar->name);
	    free(newvar);
	    return NULL;
	}
	bcopy((char *) var->val.string, (char *) newvar->val.string, var->val_len);
    }
    newvar->next_variable = NULL;
    return newvar;
}
int
snmp_synch_input(op, session, reqid, pdu, magic)
     int op;
     struct snmp_session *session;
     int reqid;
     struct snmp_pdu *pdu;
     void *magic;
{
/**    struct variable_list *var, *newvar; **/
    struct synch_state *state = (struct synch_state *) magic;
    struct snmp_pdu *newpdu;

    if (reqid != state->reqid)
	return 0;
    state->waiting = 0;
    if (op == RECEIVED_MESSAGE && (pdu->command == GET_RSP_MSG || pdu->command == REPORT_MSG)) {
	/* clone the pdu */
	state->pdu = newpdu = (struct snmp_pdu *) calloc(1, sizeof(struct snmp_pdu));
	bcopy((char *) pdu, (char *) newpdu, sizeof(struct snmp_pdu));
	newpdu->variables = NULL;
	/* clone the variables */
	if (pdu->variables != NULL) {
	    struct variable_list *var, *newvar;

	    var = pdu->variables;
	    newpdu->variables = newvar = clone_variable(var);

	    while (var->next_variable != NULL) {
		if (newvar == NULL) {
		    state->status = STAT_ERROR;
		    return 1;
		}
		newvar->next_variable = clone_variable(var->next_variable);
		var = var->next_variable;
		newvar = newvar->next_variable;
	    }
	}
	state->status = STAT_SUCCESS;
    } else if (op == TIMED_OUT) {
	state->status = STAT_TIMEOUT;
    }
    return 1;
}
#endif/***/ fe var clone, to avoid weird variable pointers */*/ replace...
     */
 *                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   ���snmplib/snmp_client.c .h   ��� <�lib/snmp_client.c~ �   �� <���F��Y��i��{���   ��� =��������������   �� =0��<��K��W��f��t   ��� =P���������������   ��
 =p��3��B��S��]��i   ���   ���������������   ����   �� =� %     D(   ��_ =�  �     B�   ��� =�  4     B�   ��� =�  H     B�   ��B >��]��m��z   ��� >(���������   ��� >@���� ��4   ��P   ��r��~���   0��� +����    ���    ��    ��#��/��> >X��V   3    /usr/people/wessels/.indent.pro                       ������������.��?��M��[��p�����������������������
����2��F��Y��i��{��������������������������������2��<��K��W��f��t����������������������������
�� ��3��B��S��]��i��s�����������������������������������)��3��C��O��_��k��t��������������������������������������)��7��B��M��]��m��z������������������������������ ��4��?��P��b��r��~������������������������������#��/��>��L��V��f��q                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          @	    /*
 * snmp_client.c - a toolkit of common functions for an SNMP client.
 *
 */
/***********************************************************
	Copyright 1988, 1989 by Carnegie Mellon University

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of CMU not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
******************************************************************/
#include <sys/types.h>
#include <sys/param.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <errno.h>
#ifdef linux
#include <string.h>
#include <stdlib.h>
#endif

#include "asn1.h"
#include "snmp.h"
#include "snmp_impl.h"
#include "snmp_api.h"
#include "snmp_client.h"

#ifndef BSD4_3
#define BSD4_2
#endif

#if !defined(BSD4_3) && !defined(linux) && !defined(sun)

typedef long fd_mask;
#define NFDBITS	(sizeof(fd_mask) * NBBY)	/* bits per mask */

#define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)	bzero((char *)(p), sizeof(*(p)))
#endif


extern int errno;

static struct synch_state snmp_synch_state;

struct snmp_pdu *
snmp_pdu_create(command)
     int command;
{
    struct snmp_pdu *pdu;

    pdu = (struct snmp_pdu *) calloc(1, sizeof(struct snmp_pdu));
    bzero((char *) pdu, sizeof(struct snmp_pdu));
    pdu->command = command;
    pdu->errstat = SNMP_DEFAULT_ERRSTAT;
    pdu->errindex = SNMP_DEFAULT_ERRINDEX;
    pdu->address.sin_addr.s_addr = SNMP_DEFAULT_ADDRESS;
    pdu->enterprise = NULL;
    pdu->enterprise_length = 0;
    pdu->variables = NULL;
    return pdu;
}

/*
 * Add a null variable with the requested name to the end of the list of
 * variables for this pdu.
 */
void
snmp_add_null_var(struct snmp_pdu *pdu, oid * name, int name_length)
{
    struct variable_list *vars;

    if (pdu->variables == NULL) {
	pdu->variables = vars = (struct variable_list *) calloc(1, sizeof(struct variable_list));
    } else {
	for (vars = pdu->variables; vars->next_variable; vars = vars->next_variable);
	vars->next_variable = (struct variable_list *) calloc(1, sizeof(struct variable_list));
	vars = vars->next_variable;
    }

    vars->next_variable = NULL;
    vars->name = (oid *) calloc(1, name_length * sizeof(oid));
    bcopy((char *) name, (char *) vars->name, name_length * sizeof(oid));
    vars->name_length = name_length;
    vars->type = ASN_NULL;
    vars->val.string = NULL;
    vars->val_len = 0;
}

#if 1 /***/			/* safe var clone, to avoid weird variable pointers */
/*
 * Clone a variable, returns NULL in case of failure
 */
static struct variable_list *
clone_variable(var)
     struct variable_list *var;
{
    struct variable_list *newvar;
    if (!var)
	return NULL;
    newvar = (struct variable_list *) calloc(1, sizeof(struct variable_list));
    if (!newvar)
	return NULL;
    bcopy((char *) var, (char *) newvar, sizeof(struct variable_list));
    if (var->name != NULL) {
	newvar->name = (oid *) calloc(1, var->name_length * sizeof(oid));
	if (newvar->name == NULL) {	/* paranoia */
	    free(newvar);
	    return NULL;
	}
	bcopy((char *) var->name, (char *) newvar->name, var->name_length * sizeof(oid));
    }
    if (var->val.string != NULL) {
	newvar->val.string = (u_char *) calloc(1, var->val_len);
	if (newvar->val.string == NULL) {	/* paranoia */
	    if (newvar->name != NULL)
		free(newvar->name);
	    free(newvar);
	    return NULL;
	}
	bcopy((char *) var->val.string, (char *) newvar->val.string, var->val_len);
    }
    newvar->next_variable = NULL;
    return newvar;
}
int
snmp_synch_input(op, session, reqid, pdu, magic)
     int op;
     struct snmp_session *session;
     int reqid;
     struct snmp_pdu *pdu;
     void *magic;
{
/**    struct variable_list *var, *newvar; **/
    struct synch_state *state = (struct synch_state *) magic;
    struct snmp_pdu *newpdu;

    if (reqid != state->reqid)
	return 0;
    state->waiting = 0;
    if (op == RECEIVED_MESSAGE && (pdu->command == GET_RSP_MSG || pdu->command == REPORT_MSG)) {
	/* clone the pdu */
	state->pdu = newpdu = (struct snmp_pdu *) calloc(1, sizeof(struct snmp_pdu));
	bcopy((char *) pdu, (char *) newpdu, sizeof(struct snmp_pdu));
	newpdu->variables = NULL;
	/* clone the variables */
	if (pdu->variables != NULL) {
	    struct variable_list *var, *newvar;

	    var = pdu->variables;
	    newpdu->variables = newvar = clone_variable(var);

	    while (var->next_variable != NULL) {
		if (newvar == NULL) {
		    state->status = STAT_ERROR;
		    return 1;
		}
		newvar->next_variable = clone_variable(var->next_variable);
		var = var->next_variable;
		newvar = newvar->next_variable;
	    }
	}
	state->status = STAT_SUCCESS;
    } else if (op == TIMED_OUT) {
	state->status = STAT_TIMEOUT;
    }
    return 1;
}
truct timeval now, earliest;
    int active = 0, requests = 0;

    timerclear(&earliest);
    /*
     * For each request outstanding, add it's socket to the fdset,
     * and if it is the earliest timeout to expire, mark it as lowest.
     */
    for(slp = Sessions; slp; slp = slp->next){
	active++;
	isp = slp->internal;
	if ((isp->sd + 1) > *numfds)
	    *numfds = (isp->sd + 1);
	FD_SET(isp->sd, fdset);
	if (isp->requests){
	    /* found another session with outstanding requests */
	    requests++;
	    for(rp = isp->requests; rp; rp = rp->next_request){
		if (!timerisset(&earliest) || timercmp(&rp->expire, &earliest, <))
		    earliest = rp->expire;
	    }
	}
    }
    if (requests == 0)	/* if none are active, skip arithmetic */
	return active;

    /*
     * Now find out how much time until the earliest timeout.  This
     * transforms earliest from an absolute time into a delta time, the
     * time left until the select should timeout.
     */
    gettimeofday(&now, (struct timezone *)0);
    earliest.tv_sec--;	/* adjust time to make arithmetic easier */
    earliest.tv_usec += 1000000L;
    earliest.tv_sec -= now.tv_sec;
    earliest.tv_usec -= now.tv_usec;
    while (earliest.tv_usec >= 1000000L){
	earliest.tv_usec -= 1000000L;
	earliest.tv_sec += 1;
    }
    if (earliest.tv_sec < 0){
	earliest.tv_sec = 0;
	earliest.tv_usec = 0;
    }

    /* if it was blocking before or our delta time is less, reset timeout */
    if (*block == 1 || timercmp(&earliest, timeout, <)){
	*timeout = earliest;
	*block = 0;
    }
    return active;
}

/* 
 * snmp_timeout should be called whenever the timeout from
 * snmp_select_info expires, but it is idempotent, so snmp_timeout can
 * be polled (probably a cpu expensive proposition).  snmp_timeout
 * checks to see if any of the sessions have an outstanding request
 * that has timed out.  If it finds one (or more), and that pdu has
 * more retries available, a new packet is formed from the pdu and is
 * resent.  If there are no more retries available, the callback for
 * the session is used to alert the user of the timeout.
 */
void
snmp_timeout()
{
    struct session_list *slp;
    struct snmp_session *sp;
    struct snmp_internal_session *isp;
#if 1 /***/ /* replacing freeme by simpler and safer rp_next*/
    struct request_list *rp, *rp_next = 0;
#endif /***/
    struct timeval now;

    gettimeofday(&now, (struct timezone *)0);
    /*
     * For each request outstanding, check to see if it has expired.
     */
    for(slp = Sessions; slp; slp = slp->  *�){
	     slp 0�ssio �(isp     p->internal;
#if 1 /***/ /* simplification */
	for(rp = isp->requests; rp; rp = rp_next){
	    rp_next = rp->next_request;
	    if (timercmp(&rp->expire, &now, <)){
		/* this timer has expired */
		if (rp->retries >= sp->retries){
#if DEBUG_SNMPTRACE
		  snmp_print_trace(slp, rp, TRACE_TIMEOUT);
#endif
		  /* No more chances, delete this entry */
		  sp->callback(TIMED_OUT, sp, rp->pdu->reqid, rp->pdu, sp->callback_magic);
		  free_one_request(isp, rp);
		  continue;
#endif /***/
		} else {
		    u_char  packet[PACKET_LENGTH];
		    int length = PACKET_LENGTH;
		    struct timeval tv;

		    /* retransmit this pdu */
		    rp->retries++;
		    rp->timeout <<= 1;
		    if (snmp_build(sp, rp->pdu, packet, &length, 0) < 0){
			fprintf(stderr, "Error building packet\n");
		    }
#if 1 /***/
		    if (snmp_dump_packet){
		      snmp_print_packet(packet, length, rp->pdu->address, 1);
		    }
#endif /***/
		    gettimeofday(&tv, (struct timezone *)0);
		    if (sendto(isp->sd, (char *)packet, length, 0, (struct sockaddr *)&rp->pdu->address, sizeof(rp->pdu->address)) < 0){
			perror("sendto");
		    }
		    rp->time = tv;
		    tv.tv_usec += rp->timeout;
		    tv.tv_sec += tv.tv_usec / 1000000L;
		    tv.tv_usec %= 1000000L;
		    rp->expire = tv;
#if DEBUG_SNMPTRACE
		    snmp_print_trace(slp, rp, TRACE_SEND);
#endif
		}
	    }
#if 1 /***/ /* simplification */
	}
#endif /***/
    }
}
t      origlength = length;
    int      ret = 0;
    u_char  *save_data;

    /* authenticates message and returns length if valid */
    data = snmp_auth_parse(data, &length, community, &community_length, &version);
    if (data == NULL)
	return -1;

   if( version != SNMP_VERSION_1 && version != SNMP_VERSION_2C && version != SNMP_VERSION_2 ) {
	    fprintf(stderr, "Wrong version: %ld\n", version);
	    return -1;
    }

    save_data = data;

    data = asn_parse_header(data, &length, &msg_type);
    if (data == NULL)
	return -1;
    pdu->command = msg_type;

    if( session->authenticator ) {
	ret = session->authenticator( origdata, origlength, save_data - community_length, 
		community_length, session, pdu );
	if( ret < 0 ) return ret;
    }

    if (pdu->command != TRP_REQ_MSG){
	data = asn_parse_int(data, &length, &type, (long *)&pdu->reqid, sizeof(pdu->reqid));
	if (data == NULL)
	    return -1;
	data = asn_parse_int(data, &length, &type, (long *)&pdu->errstat, sizeof(pdu->errstat));
	if (data == NULL)
	    return -1;
	data = asn_parse_int(data, &length, &type, (long *)&pdu->errindex, sizeof(pdu->errindex));
	if (data == NULL)
	    return -1;
    } else {
	pdu->enterprise_length = MAX_NAME_LEN;
	data = asn_parse_objid(data, &length, &type, objid, &pdu->enterprise_length);
	if (data == NULL)
	    return -1;
	pdu->enterprise = (oid *)calloc(1, pdu->enterprise_length * sizeof(oid));
	bcopy((char *)objid, (char *)pdu->enterprise, pdu->enterprise_length * sizeof(oid));

	four = 4;
	data = asn_parse_string(data, &length, &type, (u_char *)&pdu->agent_addr.sin_addr.s_addr, &four);
	if (data == NULL)
	    return -1;
	data = asn_parse_int(data, &length, &type, (long *)&pdu->trap_type, sizeof(pdu->trap_type));
	if (data == NULL)
	    return -1;
	data = asn_parse_int(data, &length, &type, (long *)&pdu->specific_type, sizeof(pdu->specific_type));
	if (data == NULL)
	    return -1;
	data = asn_parse_int(data, &length, &type, (long *)&pdu->time, sizeof(pdu->time));
	if (data == NULL)
	    return -1;
    }
    data = asn_parse_header(data, &length, &type);
    if (data == NULL)
	return -1;
    if (type != (u_char)(ASN_SEQUENCE | ASN_CONSTRUCTOR))
	return -1;
    while((int)length > 0){
	if (pdu->variables == NULL){
	    pdu->variables = vp = (struct variable_list *)calloc(1, sizeof(struct variable_list));
	} else {
	    vp->next_variable = (struct variable_list *)calloc(1, sizeof(struct variable_list));
	    vp = vp->next_variable;
	}
	vp->next_variable = NULL;
	vp->val.string = NULL;
	vp->name = NULL;
	vp->name_length = MAX_NAME_LEN;
	data = snmp_parse_var_op(data, objid, &vp->name_length, &vp->type, &vp->val_len, &var_val, (int *)&length);
	if (data == NULL)
	    return -1;
	op = (oid *)calloc(1, (unsigned)vp->name_length * sizeof(oid));
	bcopy((char *)objid, (char *)op, vp->name_length * sizeof(oid));
	vp->name = op;

	len = PACKET_LENGTH;
	switch((short)vp->type){
	    case ASN_INTEGER:
		vp->val.integer = (long *)calloc(1, sizeof(long));
		vp->val_len = sizeof(long);
		asn_parse_int(var_val, &len, &vp->type, (long *)vp->val.integer, sizeof(vp->val.integer));
		break;
	    case COUNTER:
	    case GAUGE:
	    case TIMETICKS:
	    case UINTEGER:
		vp->val.integer = (long *)calloc(1, sizeof(unsigned long));
		vp->val_len = sizeof(unsigned long);
		asn_parse_unsigned_int(var_val, &len, &vp->type, (unsigned long *)vp->val.integer, sizeof(vp->val.integer));
		break;
	    case COUNTER64:
		vp->val.counter64 = (struct counter64 *)calloc(1,  sizeof(struct counter64) );
		vp->val_len = sizeof(struct counter64);
		asn_parse_unsigned_int64(var_val, &len, &vp->type,
					 (struct counter64 *)vp->val.counter64,
					 sizeof(*vp->val.counter64));
		break;
	    case ASN_OCTET_STR:
	    case IPADDRESS:
	    case OPAQUE:
	    case NSAP:
		vp->val.string = (u_char *)calloc(1, (unsigned)vp->val_len);
		asn_parse_string(var_val, &len, &vp->type, vp->val.string, &vp->val_len);
		break;
	    case ASN_OBJECT_ID:
		vp->val_len = MAX_NAME_LEN;
		asn_parse_objid(var_val, &len, &vp->type, objid, &vp->val_len);
		vp->val_len *= sizeof(oid);
		vp->val.objid = (oid *)calloc(1, (unsigned)vp->val_len);
		bcopy((char *)objid, (char *)vp->val.objid, vp->val_len);
		break;
            case SNMP_NOSUCHOBJECT:
            case SNMP_NOSUCHINSTANCE:
            case SNMP_ENDOFMIBVIEW:
	    case ASN_NULL:
		break;
	    default:
		fprintf(stderr, "bad type returned (%x)\n", vp->type);
		break;
	}
    }
    return ret;
}

/*
 * Sends the input pdu on the session after calling snmp_build to create
 * a serialized packet.  If necessary, set some of the pdu data from the
 * session defaults.  Add a request corresponding to this pdu to the list
 * of outstanding requests on this session, then send the pdu.
 * Returns the request id of the generated packet if applicable, otherwise 1.
 * On any error, 0 is returned.
 * The pdu is freed by snmp_send() unless a failure occured.
 */
int
snmp_send(session, pdu)
    struct snmp_session *session;
    struct snmp_pdu	*pdu;
{
    struct session_list *slp;
    struct snmp_internal_session *isp = NULL;
    u_char  packet[PACKET_LENGTH];
    int length = PACKET_LENGTH;
    struct request_list *rp;
    struct timeval tv;

    for(slp = Sessions; slp; slp = slp->next){
	if (slp->session == session){
	    isp = slp->internal;
	    break;
	}
    }

    if (! pdu) {
	snmp_errno = SNMPERR_GENERR;
	return 0;
    }

    if (isp == NULL){
	snmp_errno = SNMPERR_BAD_SESSION;
	return 0;
    }
    if (pdu->command == GET_REQ_MSG || pdu->command == GETNEXT_REQ_MSG
	|| pdu->command == GET_RSP_MSG || pdu->command == SET_REQ_MSG 
	|| pdu->command == GETBULK_REQ_MSG ){
	if (pdu->reqid == SNMP_DEFAULT_REQID)
	    pdu->reqid = ++Reqid;
	if (pdu->errstat == SNMP_DEFAULT_ERRSTAT)
	    pdu->errstat = 0;
	if (pdu->errindex == SNMP_DEFAULT_ERRINDEX)
	    pdu->errindex = 0;
    } else {
	/* fill in trap defaults */
	pdu->reqid = 1;	/* give a bogus non-error reqid for traps */
	if (pdu->enterprise_length == SNMP_DEFAULT_ENTERPRISE_LENGTH){
	    pdu->enterprise = (oid *)calloc(1, sizeof(DEFAULT_ENTERPRISE));
	    bcopy((char *)DEFAULT_ENTERPRISE, (char *)pdu->enterprise, sizeof(DEFAULT_ENTERPRISE));
	    pdu->enterprise_length = sizeof(DEFAULT_ENTERPRISE)/sizeof(oid);
	}
	if (pdu->time == SNMP_DEFAULT_TIME)
	    pdu->time = DEFAULT_TIME;
    }
    if (pdu->address.sin_addr.s_addr == SNMP_DEFAULT_ADDRESS){
	if (isp->addr.sin_addr.s_addr != SNMP_DEFAULT_ADDRESS){
	    bcopy((char *)&isp->addr, (char *)&pdu->address, sizeof(pdu->address));
	} else {
	    fprintf(stderr, "No remote IP address specified\n");
	    snmp_errno = SNMPERR_BAD_ADDRESS;
	    return 0;
	}
    }
	

    if (snmp_build(session, pdu, packet, &length, 0) < 0){
	fprintf(stderr, "Error building packet\n") B�+= m  
�	/*  +�*/ B 3NMPT     */c    for testing purposes */e key!  replace...
     */
 *                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   ���snmplib/snmp_api.c c  .h   ��� <�lib/snmp_api.c~ ~  �   �� <���F��Y��i��{���   ��� =��������������   �� =0��<��K��W��f��t   ��� =P���������������   ��
 =p��3��B��S��]��i   ���   ���������������   ����   �� =� %     D(   ��_ =�  \     B�   ��� =�  4     B�   ��� =�  H     B�   ��B >��]��m��z   ��� >(���������   ��� >@���� ��4   ��P   ��r��~���   0��� 4����    ��� ����    ��#��/��> >X��V   3    /usr/people/wessels/.indent.pro                       ������������.��?��M��[��p�����������������������
����2��F��Y��i��{��������������������������������2��<��K��W��f��t����������������������������
�� ��3��B��S��]��i��s�����������������������������������)��3��C��O��_��k��t��������������������������������������)��7��B��M��]��m��z������������������������������ ��4��?��P��b��r��~������������������������������#��/��>��L��V��f��q                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        �           >X    4�                                                    $                  @             ,    2�           �     >X             ��      C             �0                              ��ude/snmp_groupvars.h~              ��ude/snmp_groupvars.h                 �X                                     ��                                     ��                                     ��                                                                     ()    /*
 * snmp_client.c - a toolkit of common functions for an SNMP client.
 *
 */
/***********************************************************
	Copyright 1988, 1989 by Carnegie Mellon University

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of CMU not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
CMU BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.
******************************************************************/
#include <sys/types.h>
#include <sys/param.h>
#include <stdio.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <errno.h>
#ifdef linux
#include <string.h>
#include <stdlib.h>
#endif

#include "asn1.h"
#include "snmp.h"
#include "snmp_impl.h"
#include "snmp_api.h"
#include "snmp_client.h"

#ifndef BSD4_3
#define BSD4_2
#endif

#if !defined(BSD4_3) && !defined(linux) && !defined(sun)

typedef long	fd_mask;
#define NFDBITS	(sizeof(fd_mask) * NBBY)	/* bits per mask */

#define	FD_SET(n, p)	((p)->fds_bits[(n)/NFDBITS] |= (1 << ((n) % NFDBITS)))
#define	FD_CLR(n, p)	((p)->fds_bits[(n)/NFDBITS] &= ~(1 << ((n) % NFDBITS)))
#define	FD_ISSET(n, p)	((p)->fds_bits[(n)/NFDBITS] & (1 << ((n) % NFDBITS)))
#define FD_ZERO(p)	bzero((char *)(p), sizeof(*(p)))
#endif


extern int errno;

static struct synch_state snmp_synch_state;

struct snmp_pdu *
snmp_pdu_create(command)
    int command;
{
    struct snmp_pdu *pdu;

    pdu = (struct snmp_pdu *)calloc(1, sizeof(struct snmp_pdu));
    bzero((char *)pdu, sizeof(struct snmp_pdu));
    pdu->command = command;
    pdu->errstat = SNMP_DEFAULT_ERRSTAT;
    pdu->errindex = SNMP_DEFAULT_ERRINDEX;
    pdu->address.sin_addr.s_addr = SNMP_DEFAULT_ADDRESS;
    pdu->enterprise = NULL;
    pdu->enterprise_length = 0;
    pdu->variables = NULL;
    return pdu;
}

/*
 * Add a null variable with the requested name to the end of the list of
 * variables for this pdu.
 */
void
snmp_add_null_var(struct snmp_pdu *pdu, oid *name, int name_length)
{
    struct variable_list *vars;

    if (pdu->variables == NULL){
	pdu->variables = vars = (struct variable_list *)calloc(1, sizeof(struct variable_list));
    } else {
	for(vars = pdu->variables; vars->next_variable; vars = vars->next_variable)
	    ;
	vars->next_variable = (struct variable_list *)calloc(1, sizeof(struct variable_list));
	vars = vars->next_variable;
    }

    vars->next_variable = NULL;
    vars->name = (oid *)calloc(1, name_length * sizeof(oid));
    bcopy((char *)name, (char *)vars->name, name_length * sizeof(oid));
    vars->name_length = name_length;
    vars->type = ASN_NULL;
    vars->val.string = NULL;
    vars->val_len = 0;
}

#if 1 /***/ /* safe var clone, to avoid weird variable pointers */
/*
 * Clone a variable, returns NULL in case of failure
 */
static   struct variable_list *
clone_variable(var)
  struct variable_list *var;
{
  struct variable_list *newvar;
  if(! var) return NULL;
  newvar = (struct variable_list *)calloc(1, sizeof(struct variable_list));
  if(! newvar) return NULL;
  bcopy((char *)var, (char *)newvar, sizeof(struct variable_list));
  if (var->name != NULL){
    newvar->name = (oid *)calloc(1, var->name_length * sizeof(oid));
    if(newvar->name == NULL){ /* paranoia */
      free(newvar);
      return NULL;
    }
    bcopy((char *)var->name, (char *)newvar->name, var->name_length * sizeof(oid));
  }
  if (var->val.string != NULL){
    newvar->val.string = (u_char *)calloc(1, var->val_len);
    if(newvar->val.string == NULL){ /* paranoia */
      if(newvar->name != NULL) free(newvar->name);
      free(newvar);
      return NULL;
    }
    bcopy((char *)var->val.string, (char *)newvar->val.string, var->val_len);
  }
  newvar->next_variable = NULL;
  return newvar;
}
int
snmp_synch_input(op, session, reqid, pdu, magic)
    int op;
    struct snmp_session *session;
    int reqid;
    struct snmp_pdu *pdu;
    void *magic;
{
  /**    struct variable_list *var, *newvar; **/
    struct synch_state *state = (struct synch_state *)magic;
    struct snmp_pdu *newpdu;

    if (reqid != state->reqid)
	return 0;
    state->waiting = 0;
    if (op == RECEIVED_MESSAGE && (pdu->command == GET_RSP_MSG || pdu->command == REPORT_MSG) ){
	/* clone the pdu */
	state->pdu = newpdu = (struct snmp_pdu *)calloc(1, sizeof(struct snmp_pdu));
	bcopy((char *)pdu, (char *)newpdu, sizeof(struct snmp_pdu));
	newpdu->variables = NULL;
	/* clone the variables */
	if (pdu->variables != NULL){
          struct variable_list *var, *newvar;

	  var = pdu->variables;
	  newpdu->variables = newvar = clone_variable(var);

	  while(var->next_variable != NULL){
	    if(newvar == NULL){
	      state->status =  STAT_ERROR;
	      return 1;
	    }
	    newvar->next_variable = clone_variable(var->next_variable);
	    var = var->next_variable;
	    newvar = newvar->next_variable;
	  }
	}
	state->status = STAT_SUCCESS;
    } else if (op == TIMED_OUT){
	state->status = STAT_TIMEOUT;
    }
    return 1;
}
#endif /***/

/*
 * If there was an error in the input pdu, creates a clone of the pdu
 * that includes all the variables except the one marked by the errindex.
 * The command is set to the input command and the reqid, errstat, and
 * errindex are set to default values.
 * If the error status didn't indicate an error, the error index didn't
 * indicate a variable, the pdu wasn't a get response message, or there
 * would be no remaining variables, this function will return NULL.
 * If everything was successful, a pointer to the fixed cloned pdu will
 * be returned.
 */
struct snmp_pdu *
snmp_fix_pdu(pdu, command)
     struct snmp_pdu *pdu;
     int command;
{
    struct variable_list *var, *newvar;
    struct snmp_pdu *newpdu;
    int index, copied = 0;

    if (pdu->command != GET_RSP_MSG || pdu->errstat == SNMP_ERR_NOERROR || pdu->errindex <= 0)
	return NULL;
    /* clone the pdu */
    newpdu = (struct snmp_pdu *) calloc(1, sizeof(struct snmp_pdu));
    bcopy((char *) pdu, (char *) newpdu, sizeof(struct snmp_pdu));
    newpdu->variables = 0;
    newpdu->command = command;
    newpdu->reqid = SNMP_DEFAULT_REQID;
    newpdu->errstat = SNMP_DEFAULT_ERRSTAT;
    newpdu->errindex = SNMP_DEFAULT_ERRINDEX;
    var = pdu->variables;
    index = 1;
    if (pdu->errindex == index) {	/* skip first variable */
	var = var->next_variable;
	index++;
    }
    if (var != NULL) {
	newpdu->variables = newvar = (struct variable_list *) calloc(1, sizeof(struct variable_list));
	bcopy((char *) var, (char *) newvar, sizeof(struct variable_list));
	if (var->name != NULL) {
	    newvar->name = (oid *) calloc(1, var->name_length * sizeof(oid));
	    bcopy((char *) var->name, (char *) newvar->name, var->name_length * sizeof(oid));
	}
	if (var->val.string != NULL) {
	    newvar->val.string = (u_char *) calloc(1, var->val_len);
	    bcopy((char *) var->val.string, (char *) newvar->val.string, var->val_len);
	}
	newvar->next_variable = 0;
	copied++;

	while (var->next_variable) {
	    var = var->next_variable;
	    if (++index == pdu->errindex)
		continue;
	    newvar->next_variable = (struct variable_list *) calloc(1, sizeof(struct variable_list));
	    newvar = newvar->next_variable;
	    bcopy((char *) var, (char *) newvar, sizeof(struct variable_list));
	    if (var->name != NULL) {
		newvar->name = (oid *) calloc(1, var->name_length * sizeof(oid));
		bcopy((char *) var->name, (char *) newvar->name, var->name_length * sizeof(oid));
	    }
	    if (var->val.string != NULL) {
		newvar->val.string = (u_char *) calloc(1, var->val_len);
		bcopy((char *) var->val.string, (char *) newvar->val.string, var->val_len);
	    }
	    newvar->next_variable = 0;
	    copied++;
	}
    }
    if (index < pdu->errindex || copied == 0) {
	snmp_free_pdu(newpdu);
	return NULL;
    }
    return newpdu;
}


int
snmp_synch_response(ss, pdu, response)
     struct snmp_session *ss;
     struct snmp_pdu *pdu;
     struct snmp_pdu **response;
{
    static struct synch_state *state = &snmp_synch_state;
    int numfds, count;
    fd_set fdset;
    struct timeval timeout, *tvp;
    int block;

    /* avoid crashes in case of timeout */
    *response = NULL;
    state->pdu = NULL;

    if (!pdu) {
	return STAT_ERROR;
    }
    if ((state->reqid = snmp_send(ss, pdu)) == 0) {
	snmp_free_pdu(pdu);
	return STAT_ERROR;
    }
    state->waiting = 1;

    while (state->waiting) {
	numfds = 0;
	FD_ZERO(&fdset);
	block = 1;
	tvp = &timeout;
	timerclear(tvp);
	snmp_select_info(&numfds, &fdset, tvp, &block);
	if (block == 1)
	    tvp = NULL;		/* block without timeout */
	count = select(numfds, &fdset, 0, 0, tvp);
	if (count > 0) {
	    snmp_read(&fdset);
	} else
	    switch (count) {
	    case 0:
		snmp_timeout();
		break;
	    case -1:
		if (errno == EINTR) {
		    continue;
		} else {
		    perror("select");
		}
		/* FALLTHRU */
	    default:
		return STAT_ERROR;
	    }
    }
    *response = state->pdu;
#ifdef linux
    if (!*response)
	return STAT_ERROR;
#endif
    return state->status;
}

void
snmp_synch_setup(struct snmp_session *session)
{
    session->callback = snmp_synch_input;
    bzero((char *) &snmp_synch_state, sizeof(snmp_synch_state));
    session->callback_magic = (void *) &snmp_synch_state;
}

char *error_string[6] =
{
    "No Error",
    "Response message would have been too large.",
    "There is no such variable name in this MIB.",
    "The value given has the wrong type or length",
    "This variable is read only",
    "A general failure occured"
};

char *
snmp_errstring(errstat)
     int errstat;
{
    if (errstat <= SNMP_ERR_GENERR && errstat >= SNMP_ERR_NOERROR) {
	return error_string[errstat];
    } else {
	return "Unknown Error";
    }
}
