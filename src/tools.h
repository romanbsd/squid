
/* $Id: tools.h,v 1.6 1996/03/27 20:21:15 wessels Exp $ */

extern char *getMyHostname _PARAMS((void));
extern int getMaxFD _PARAMS((void));
extern int safeunlink _PARAMS((char *path, int quiet));
extern void death _PARAMS((int sig));
extern void fatal _PARAMS((char *message));
extern void fatal_dump _PARAMS((char *message));
extern void rotate_logs _PARAMS((int sig));
extern void shut_down _PARAMS((int sig));
extern void sig_child _PARAMS((int sig));
extern void check_suid _PARAMS((void));
extern int daemonize _PARAMS((void));
extern void writePidFile _PARAMS((void));
extern void setMaxFD _PARAMS((void));

extern int do_mallinfo;
