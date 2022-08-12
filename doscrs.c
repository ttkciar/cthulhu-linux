#include "doscrs.h"
#include <varargs.h>

char *_DC_scrn_base, _DC_display_visible=1;
int _DC_loc_x = 0, _DC_loc_y = 0;

void clrtoeol(void) {
	int i = _DC_loc_x;
	for (; _DC_loc_x < COLS; _DC_loc_x++)
	{
		addchr(' ');
	}
	_DC_loc_x = i;
}

void clrtobot(void) {
	int x = _DC_loc_y, y = _DC_loc_y;
	clrtoeol();
	_DC_loc_x = 0;
	for (;_DC_loc_y < LINES; _DC_loc_y++)
		for(_DC_loc_x = 0; _DC_loc_x < COLS; _DC_loc_x++)
		{
			addchr(' ');
		}
	_DC_loc_x = x;
	_DC_loc_y = y;
}

void addstr(char *string) {
	while (1)
	{
	    switch(*string)
	    {
		case '\0':
		    return;
		    break;
		case '\t':
		    _DC_loc_x += (8 - (_DC_loc_x % 8));
		    if (++_DC_loc_x >= COLS)
		    {
			    _DC_loc_x = 0;
			    _DC_loc_y = (_DC_loc_y + 1) % LINES;
		    }
		    break;
		case '\n':
		    _DC_loc_x = 0;
		    _DC_loc_y = (_DC_loc_y + 1) % LINES;
		    break;
		default:
		    addchr(*string);
		    if (++_DC_loc_x >= COLS)
		    {
			    _DC_loc_x = 0;
			    _DC_loc_y = (_DC_loc_y + 1) % LINES;
		    }
		    break;
	    }
	    string++;
	}
}

void move(int y, int x) {
	_DC_loc_x = x;
	_DC_loc_y = y;
}

void refresh(void) { }

void clear(void) {
	write(1, "\f", 1);
}

static void _DC_rel_screen(void) {
	signal(SIG_REL, _DC_rel_screen);

	_DC_display_visible=0;

	ioctl(0, VT_RELDISP, VT_TRUE);
}


static void _DC_acq_screen(void) {
	signal(SIG_ACQ, _DC_acq_screen);

	_DC_display_visible=1;

	ioctl(0, VT_RELDISP, VT_ACKACQ);
}


void initscr(void) {
	int _DC_Oldmode;
	struct vt_mode _DC_smode;
	void _DC_init_kbd();

	clear();
	_DC_scrn_base= (char *) ioctl(0, MAPCONS, (char *)0);

	signal(SIG_REL, _DC_rel_screen);
	signal(SIG_ACQ, _DC_acq_screen);
	signal(2, exit);

	_DC_Oldmode = ioctl(0, CONS_GET, (char *)0);

	_DC_smode.mode = VT_PROCESS;
	_DC_smode.waitv = 0;		/* not implemented */
	_DC_smode.relsig = SIG_REL;
	_DC_smode.acqsig = SIG_ACQ;
	_DC_smode.frsig  = SIGINT;		/* not implemented */

	ioctl(0, VT_SETMODE, &_DC_smode);
	_DC_init_kbd();
}


void _DC_multi_screen(int _DC_screen_num) {
	ioctl(0, VT_ACTIVATE, _DC_screen_num);
}


static struct termio _DC_save_tty;

void cdecl _DC_shutdown_keys(void) {
	ioctl(0, TCSETA, &_DC_save_tty);
	ioctl(0, KDSKBMODE, K_XLATE);
	close(0);
	open("/dev/tty", O_RDONLY);
}

void _DC_init_kbd(void) {
	struct termio _DC_tty;

	close(0);
	open("/dev/tty", O_NDELAY|O_RDONLY);

	ioctl(0, TCGETA, &_DC_tty);
	_DC_save_tty = _DC_tty;
	/* atexit(_DC_shutdown_keys); */

	_DC_tty.c_lflag &= ~ (ICANON | XCASE | ECHO);
	_DC_tty.c_iflag = 0;
	_DC_tty.c_cc[VMIN] = 0;
	_DC_tty.c_cc[VTIME] = 0;
	ioctl(0, TCSETA, &_DC_tty);
	ioctl(0, KDSKBMODE, K_RAW);
}
/*

void _DC_init_main_buffer(void) {
	register struct buf_st *_DC_ptr;

	_DC_ptr= zap("main");
	_DC_set_wrk_buffer(_DC_ptr);

	don't set cur_window before call to init_window()!
	_DC_cur_window= _DC_window[0]= _DC_init_window(ptr);
	_DC_disp_cur_buffer();
}

*/

char _DC_window_string[256];

void printw(va_dcl va_alist) { // zzapp
	va_list args;
	char *fmt;

	va_start(args);
	fmt = va_arg(args, char *);
	(void) vsprintf(_DC_window_string, fmt, args);
	addstr(_DC_window_string);
	va_end(args);
}
