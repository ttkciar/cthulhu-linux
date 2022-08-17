#include "cthulhu.h"
#include "clock.h"
#include "init.h"
#include "heal.h"
#include "misc.h"
#include "monsters.h"
#include "graphics.h"

/*
 * If the player is travelling then he/she gets to rest while doing so. 
 * ("Travel is good for the soul.") For each hour spent travelling a
 * rest() call is made. Note that different forms of travel recuperate
 * the traveller faster.
 */
void traveltime(int hours, int type) {
	register int i;
	
	player.time += hours;
	COUNT_DOWN ( i, hours ) 
		rest(type);
}

/*
 * This is quick way we burn off a set number of peacefull (non-interupted by
 * monsters or corrupted earth) hours. It increments the global corruption as
 * well as planting monsters and treasure/objects.
 */
void burntime(int hours) {
	while (hours-- > 0)
		if (++player.time % 10 == 0) 
		{
			player.total_corruption++;
			if (player.time % GROWTH == 0) 
			{
				if (rand() % corr(-1) > 100) 
					cty[player.city].mon++;
				if (rand() % corr(-1) > 100)
					cty[player.city].tres++;
			}
		}
}

void eat_something(void) {
        int longitude = cty[player.city].lon;

        if (longitude > 360)        ask("nothing to eat here");
        else if (player.money < 10) ask("can't afford to eat");
        else if (player.food < 5)   ask("not that hungry");
        else {
                player.money -= 10;
                player.food  -=  5;
        }
}

/*
 * This is a cron-like subroutine that's called every hour. It ticks off
 * the time that the player's been in a certain area and then sicks a monster
 * or objects on his/her city. Other time related things (plauges) are dealt
 * with here. The return value is set to zero if nothing of interest happens
 * (even losing health to a plauge). Yet it returns non-zero if a monster 
 * attacks or the location starts to attack the user.
 */
int CthulhuClock(void) {
	register int i;
	int ck=0;

	ck=monster(0);
	if (++player.time % 10 == 0) 
	{
	    player.total_corruption++;
	    if (player.time % 20 == 0) 
	    {
		if (player.money > 9 && cty[player.city].lon < 361) 
		{
		    player.money -= 10;
		    if (player.food > 0) 
			    player.food -= (1+ (player.food / 10));
		} 
		else 
		{
		    player.food++;
		    if (player.food > 30) 
		    {
			player.body--;
			player.topbody -= 2;
		    }
		}
		rest(5);
		if (player.time % GROWTH == 0) 
		{
		    if (rand() % corr(-1) > 100) 
			    cty[player.city].mon++;
		    if (rand() % corr(-1) > 100)
			    cty[player.city].tres++;
		    COUNT_DOWN (i, NPLAGUE)
			if (plague[i].caught == 1) 
			{
			    ++player.feddisease;
			    if ((player.feddisease %= 20 ) == 0) 
			    {
				int c = 0;
				while (c < NPLAGUE) 
				{
					if (plague[c].caught > 0) 
					{
						plague[c].caught = 0;
						c = NPLAGUE;
					}
					c++;
				}
				STATE("You feel less diseased,");
				STATE("   tenacious aren't you.");
			    }
			    if (player.longevity == 0) 
			    {
				if (i<7) 
				{
				    player.body -= (player.body>500)?100:10;
				    player.topbody = (player.topbody * 9) / 11;
				} 
				else 
				{
				    player.sanity -= (player.sanity>500)?100:10;
				    player.topsan = (player.topsan * 9) / 11;
				} 
				sprintf(death, "died of %s", plague[i].name);
			    }
			}		/* Do I have plague number i */
		}			/* GROWTH cycle */
	    }				/* Every Day */
	}				/* Main clock cycle */

	if (cty[player.city].lon > 360 			/* Off world */
	    && player.longevity == 0 			/* no longevity spell */
	    && player.drunk != cty[player.city].lon-360)  /* and no drunk cup */
	{
	    int envdamge = 5;
	    ck=1;
	    switch (cty[player.city].lon-360) 
	    {
	    case 1: 
		STATE("Water begins to fill your lungs.");
		player.body -= envdamge;
		break;
	    case 2:
		STATE("The sand-storm begins to rip the flesh from your body.");
		player.body -= envdamge;
		break;
	    case 3:
		STATE("The alien atmosphere begins to choke you.");
		player.body -= envdamge;
		break;
	    case 4:
		STATE("The howling in your ears is driving you insane.");
		player.sanity -= envdamge;
		break;
	    case 5:
		STATE("Your mind begins to dissimilate, being in Limbo");
		player.sanity -= envdamge;
		break;
	    case 6:
		STATE("Your flesh begins to freeze solid.");
		player.body -= envdamge;
		break;
	    case 7:
		STATE("Your body begins to dissipate.");
		player.body -= envdamge;
		break;
	    case 8:
		STATE("The lands and woods themselves have turn against you.");
		player.body -= envdamge;
		break;
	    case 9:
		STATE("The winds begin to chill your bone to the marrow.");
		player.body -= envdamge;
		break;
	    case 10:
		STATE("The vacuum of space begins to rip your body apart.");
		player.body -= envdamge;
		break;
	    default:
		STATE("It's kinda spooky here.");
		break;
	    }
	    strcpy(death, "stayed too long");
	} 
	else if (cty[player.city].lon < 360		/* We're on earth */
		&& player.longevity == 0 		/* without longevity */
		&& corr(-1) > 4000) 			/* and it's corrupt */
	{
		ck=1;
		STATE("The earth rejects you for being too holy.");
		player.body -= 10;
		player.sanity -= 10;
		strcpy(death, "Too holy for Mother Earth");
	}
	return(ck);
}
