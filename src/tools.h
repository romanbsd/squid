/* $Id: tools.h,v 1.2 1996/03/27 04:42:11 wessels Exp $ */

extern char *getMyHostname _PARAMS((void));
extern int getMaxFD _PARAMS((void));
extern int safeunlink _PARAMS((char *path, int quiet));
extern void death _PARAMS((int sig));
extern void fatal _PARAMS((char *message));
extern void fatal_dump _PARAMS((char *message));
extern void rotate_logs _PARAMS((int sig));
extern void shut_down _PARAMS((int sig));

extern int do_mallinfo;
