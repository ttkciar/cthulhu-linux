#ifndef INIT_H
#define INIT_H
#include "cthulhu.h"

extern struct plague_table plague[NPLAGUE];
extern char *dlev[NCORRUPTION];
extern char *headmaster[NDIMS];

extern unsigned int goal;
extern unsigned int end_goal;

extern struct fight_status ft_stat;

extern struct player_table player;
extern struct city_table cty[NCITIES];
extern struct object_table obj[NOBJECTS];
extern struct monster_table mon[NMONSTERS];
extern struct spell_table spell[NSPELLS];
extern int highlist;

extern char death[MAXPATH];
extern char tmp[MAXPATH];

extern int pc, st_pos, mpd;
extern int cpc[NCITIES], tpc[NCITIES], cp[NCITIES];
extern int spl[NSPELLS];
extern int object[NOBJECTS];
extern int sign_index[NUMBERSIGNS];

#endif
