#include "start_game.h"
#include "cthulhu.h"
#include "init.h"
#include "misc.h"
#include "graphics.h"
#include "monsters.h"

/*
 * This function is called upon to sort the objects by their worth.
 */
int sort_object(const void *m1p, const void *m2p) {
        const struct object_table *m1 = m1p;
        const struct object_table *m2 = m2p;
	if (m1->worth > m2->worth)
		return 1;
	if (m1->worth < m2->worth)
		return -1;
	return 0;
}

/*
 * This function is called upon to sort the objects by their lethiality.
 */
int sort_monster(const void *m1p, const void *m2p) {
        const struct monster_table *m1 = m1p;
        const struct monster_table *m2 = m2p;
	if (m1->level > m2->level)
		return 1;
	if (m1->level < m2->level)
		return -1;
	return 0;
}

/*
 * Preps the Monsters, Objects and the character for playing in the game.
 * Make special note that the preping of the monsters and objects is done
 * at the begining of each and every game, a carry over from when the monsters
 * could be (planned to be) different for each and every game. A brainiac can
 * either implement this again (yet) or seperate the startup-code into a
 * one time only data file that's run.
 */
void startgame(void) {
	register int i, c;
	int itmp;
	char *env_value;
	char *getenv();

#ifdef DEBUG
	FILE *string;

	string=fopen(DATAFILE, "w");
#endif
	move(10, 20); clrtoeol();
	printw("%s%s%s%s","Welcome, ",player.name,", to cthulhu/Linux v",VERSION);
#ifdef DEBUG
	addstr("\nWarning, DEBUG option enacted.");
	addstr("\nNo high score will be recorded.");
#endif
	if ((env_value = getenv(ENV_STR)) != (char *) NULL) 
	{
		itmp = atoi(env_value);
		if (itmp>=8 && itmp<15) 
			player.str = itmp;
	}
	if ((env_value = getenv(ENV_DEX)) != (char *) NULL) 
	{
		itmp = atoi(env_value);
		if (itmp>=8 && itmp<15) 
			player.dex = itmp;
	}
	if ((env_value = getenv(ENV_INT)) != (char *) NULL) 
	{
		itmp = atoi(env_value);
		if (itmp>=8 && itmp<15) 
			player.intel = itmp;
	}
	if (player.str + player.intel + player.dex != 30) 
	{
		player.str = 10;
		player.dex = 10;
		player.intel = 10;
	}
	WAIT();
	clear();
	c=400;
	itmp=NCITIES - 1;
	COUNT_DOWN (i, NCITIES) 
	{
		cty[i].mon = MOD_4(rand()) + 1 + cty[i].level;
		cty[i].tres = MOD_4(rand()) + 1 + cty[i].level;
		cty[i].level = cty[i].level * 200 + 
			((cty[i].level == 0) ? rand()%100 : rand()%300);
		if (corr(i) < c) 
		{
			itmp=i;
			c=corr(i);
		}
	}
	player.city = itmp;
	fillscreen();
	COUNT_DOWN (i, NMONSTERS ) 
	{
		itmp = 1 + nsqrt(mon[i].body);
		if POWER(i,'C') 
			itmp += 4;
		itmp *= (2 * mon[i].dice * (mon[i].dam + 1));
		itmp *= ((int)(mon[i].dex + 4 ) / 5);
		if POWER(i,'A') itmp = (itmp * 4);
		if POWER(i,'a') itmp = (itmp * 3) / 2;
		if POWER(i,'c') itmp = (itmp * 5);
		if POWER(i,'d') itmp = (itmp * 7) / 5;
		if POWER(i,'D') itmp = (itmp * 135) / 100;
		if POWER(i,'E') itmp = (itmp * 7) / 5;
		if POWER(i,'h') itmp = (itmp * 3) / 2;
		if POWER(i,'H') itmp = (itmp * 9) / 5;
		if POWER(i,'i') itmp = (itmp * 2);
		if POWER(i,'I') itmp = (itmp * 9) / 5;
		if POWER(i,'p') itmp = (itmp * 135) / 100;
		if POWER(i,'P') itmp = (itmp * 7) / 2;
		if POWER(i,'r') itmp = (itmp * 9) / 5;
		if POWER(i,'R') itmp = (itmp * 6) / 5;
		if POWER(i,'s') itmp = (itmp * 5) / 3;
		if POWER(i,'t') itmp = (itmp * 4) / 3;
		if POWER(i,'u') itmp = (itmp * 3) / 2;
		if POWER(i,'v') itmp = (itmp * (int)(110 + 40 * mon[i].dice)) / 100;
		if POWER(i,'V') itmp = (itmp * 3) / 2;
		if POWER(i,'W') itmp = (itmp * (POWER(i, 'P')?6:3)) / 2;
		itmp += mon[i].san * 3;
		mon[i].level = itmp;
#ifdef DEBUG
		fprintf(string,"%d %s\n", itmp, mon[i].name);
#endif
	}
	qsort((char *) &mon[0],
              NMONSTERS,
              sizeof(struct monster_table), 
              sort_monster);
	COUNT_DOWN (i, NOBJECTS) 
	{
#ifdef DEBUG
		obj[i].have = (rand() % 3 >0)?2:0;
		if (obj[i].have> 0) 
			player.objects++;
		if (obj[i].charges == 0) 
		{
			if GIFT(i,'a') player.ability[AARMOR]=1;
			if GIFT(i,'T') player.ability[ASEEINVIS]=1;
			if GIFT(i,'w') player.ability[AWILL]=1;
			if GIFT(i,'m') player.ability[AMENTALHEALTH]=1;
			if GIFT(i,'h') player.ability[AHOLY]=1;
			if GIFT(i,'H') player.ability[ALEARN]=1;
		}
#endif
		itmp = obj[i].sanlos * 5;
		if (GIFT(i,'s')) itmp += 60;
		if (GIFT(i,'P')) itmp += 80;
		if (GIFT(i,'i')) itmp += 90;
		if (GIFT(i,'d')) itmp += 60;
		if (GIFT(i,'r')) itmp += 60;
		if (GIFT(i,'S')) itmp += 60;
		if (GIFT(i,'t')) itmp += 100;
		if (GIFT(i,'D')) itmp += 150;
		if (GIFT(i,'a')) itmp += 300;
		if (GIFT(i,'m')) itmp += 150;
		if (GIFT(i,'h')) 
		{
			itmp += 90;
			if (obj[i].dice == 1) 
				itmp += 100;
		}
		if (GIFT(i,'k')) itmp += 90;
		if (GIFT(i,'H')) itmp += 160;
		if (GIFT(i,'g')) itmp += 90;
		if (GIFT(i,'v')) itmp += 60;
		if (GIFT(i,'C')) itmp += 90;
		if (GIFT(i,'$')) itmp += 10;
		if (GIFT(i,'w')) itmp += 900;
		if (GIFT(i,'T')) itmp += 170;
		if (GIFT(i,'G')) itmp += 100;
		if (GIFT(i,'b')) itmp += 350;
		if (GIFT(i,'B')) itmp += 400;
		if (GIFT(i,'E')) itmp += 450;
		if (GIFT(i,'c')) itmp += 300;
		if (GIFT(i,'1')) itmp += 30;
		if (GIFT(i,'2')) itmp += 60;
		if (GIFT(i,'3')) itmp += 90;
		if (GIFT(i,'4')) itmp += 120;
		if (GIFT(i,'5')) itmp += 150;
		if (GIFT(i,'6')) itmp += 180;
		if (GIFT(i,'7')) itmp += 210;
		if (GIFT(i,'8')) itmp += 240;
		if (GIFT(i,'9')) itmp += 270;
		if (GIFT(i,'0')) itmp += 300;
		if (obj[i].lastused > 0 && obj[i].charges > 0) 
			itmp = (itmp * 1000) / obj[i].charges;
		else if (obj[i].charges > 0) 
		{
			obj[i].charges = obj[i].charges * (3 + (rand() % 3));
			itmp = itmp * obj[i].charges;
		} 
		else if (obj[i].charges < 0) 
		{
			itmp += (obj[i].lastused * 2);
		}
		if (GIFT(i,'n')) 
			itmp += (obj[i].dice * (obj[i].dam - 3)*80);
		else if (obj[i].dice > 1) 
			itmp += (obj[i].dice * (obj[i].dam - 1)*90);
		obj[i].worth = itmp - 30;
#ifdef DEBUG
		fprintf(string,"%d %s\n", itmp, obj[i].name);
#endif
	}
	qsort((char *) &obj[0],
              NOBJECTS,
              sizeof(struct object_table), 
              sort_object);
#ifdef DEBUG
	close(string);
#endif
}

/*
 * Set the system up for play. Basicly signal traps and screen modes.
 */
void begin(void) {
#ifdef NEVER
	signal(2, Quit);	/* 2 = SIGnal INTerupt	*/
	signal(10, buserr);	/* 10 = Bus error */
	signal(11, segerr);	/* 11 = Segmentation fault error */
#endif
	initscr();
#ifndef NOCURSES
	crmode();
	noecho();
#endif
}
