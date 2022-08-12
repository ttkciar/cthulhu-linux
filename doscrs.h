#include <signal.h>
#include <assert.h>
#ifndef NODEBUG
#include <stdio.h>
#include <stdlib.h>
#endif

#include <fcntl.h>
#include <memory.h>

#include <sys/types.h>
//#include <sys/console.h>
//#include <sys/vtkd.h>
//#include <sys/termio.h>

#define SIG_REL	SIGUSR1
#define SIG_ACQ	SIGUSR2

#define size_of_char	sizeof(char)

extern char *_DC_scrn_base;
extern char _DC_display_visible;
extern int _DC_loc_x;
extern int _DC_loc_y;

#define addchr(c)	\
	*((char *)(_DC_scrn_base + \
		  2 * size_of_char * (_DC_loc_x + _DC_loc_y * COLS)))=(c)
#ifndef LINES
#define LINES	23
#endif
#ifndef COLS
#define COLS	80
#endif
