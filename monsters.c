#include "monsters.h"
#include "cthulhu.h"
#include "init.h"
#include "misc.h"
#include "graphics.h"
#include "combat.h"
#include "end_game.h"

extern unsigned int end_goal;

void Majorsummoning(int i) {
	register int j;

	if (goal & (0x1 << i))
	{
		STATE("There's nothing to summon.");
	} 
	else 
	{
		cty[player.city].mon++;
		fight(match_head_to_monster(headmaster[i]), 0);
	}
	if (player.body > 0) 
		goal |= (0x1 << i);
	if (goal == end_goal)
	{
		clear();
		STATE("The thrill of making earth safe has driven you insane.");
		WAIT();
		sprintf(death, "won the game in %d months, %d days", 
			player.time/720, (player.time/24) % 30);
		player.score = 10000000 - (25 * player.time);
		if (player.score < 0) 
			player.score = 0;
		if (player.score < 2000000) 
			player.score = (player.score + 2000000) / 2;
		player.sanity = 0;
		endgame(0);
	}
}

int monster(int odds) {
	int weight;

	weight = cty[player.city].mon - ((player.stealth>0)?3:0)
		- ((2*player.intel + player.dex) / 20);
	odds += ((weight>0)?weight:0) *
		(corr(-1) + 150) / 35 - (rand() % 100);
	if (odds>0 && cty[player.city].mon>0) 
	{
		fight(getmonster(corr(-1), 0), 0);
		player.stealth = 0;
		player.longevity = 0;
		return(1);
	} 
	else 
		return(0);
}

int getmonster(int level, int s) {
        // s == ranking of monster and s=1 if summoned by another monster
	register int i;
	int flag, d;
	int lowi, maxi;

	do {
		lowi = -1;
		flag = 1;
		d = level;
		COUNT_DOWN (i, NMONSTERS) 
		{
			if (flag == 1) 
			{
				maxi = i;
				if (mon[i].level < d)
				{
				    flag = 0;
				    d = d / 2;
				}
			}
			else
			{
				if (mon[i].level > d)
				    lowi = i;
				else
				    break;
			}
		}
		if (lowi != -1)
			maxi = lowi + (rand() % (maxi - lowi));
		level -= 500;		/* In case of headmaster loop */
	} while ((s==1 && POWER(maxi, 's')) || POWER(maxi, 'G'));

	player.ability[AREGEN] = 0;
	mpd = mon[maxi].body;
	return(maxi);
}

int match_head_to_monster(char *name) {
	register int i;

	COUNT_DOWN(i, NMONSTERS)
		if (0 == strcmp(name, mon[i].name))
			return i;
	return -1;
}
