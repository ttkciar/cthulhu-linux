#include "misc.h"
#include "cthulhu.h"
#include "init.h"
#include "graphics.h"

/*
 * Integer square root routine. Not that important cause it only get's called
 * at the beginning and we shouldn't worry about loading in math packages for
 * just this. A braniac can put in his/her favorite SQRT routine to speed this
 * up in times of despiration.
 */
int nsqrt(int num) {
	register int sqr=0;

	while (sqr*sqr <= num) 
		sqr++;
	return(sqr - 1);
}

/*
 * Return the corruption level of the city in question. That city being 
 * specified by the argument, or the current city if the argument is set
 * to negative one.
 */
int corr(int city) {
	if (city == -1) 
		return player.total_corruption + cty[player.city].level;
	else 
		return player.total_corruption + cty[city].level;
}

/*
 * Roll a disease for the player. If they already have it, don't tell
 * them about it.
 */
void disease(int y) {
	register int i;

	i = rand() % NPLAGUE;
	if (plague[i].caught != 1) 
	{
		plague[i].caught++;
		sprintf(tmp, "You have been diseased with %s", plague[i].name);
		STATE(tmp);
                WAIT();
	}
}

/*
 * Return descriptive phase on how corrupt the current town is. These phrases
 * are stored in the array dlev[], and each level is twice as bad as the
 * previous one.
 */
char *dclevel(int l) {
	int l1=0;
	while(l > 100 && l1 < (NCORRUPTION-1)) 
	{
		l1++;
		l = l/2;
	}
	return(dlev[l1]);
}
