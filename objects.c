#include "objects.h"
#include "cthulhu.h"
#include "init.h"
#include "misc.h"
#include "graphics.h"
#include "end_game.h"
#include "monsters.h"
#include "combat.h"
#include "travel.h"
#include "spell.h"

void displayobj(int type) { // 0=any 1=weapon 2=drink 3=lookup 4=useobj 5=Eldersign
	int i, j;

        composelist(NULL);      /* start a list */
	j = 0;
	COUNT_DOWN (i, NOBJECTS) 
	if (obj[i].have>0 
	    && (type != 1 || (int) (obj[i].dice + obj[i].dam) > 2) 
	    && (type != 2 || 
		    (obj[i].abilities >= '0' && obj[i].abilities <= '9'))
	    && (type != 3 || obj[i].have == 1) 
	    && (type != 4 || obj[i].charges < 0 || 
		    (obj[i].charges > 0 && (obj[i].lastused == 0 || 
			    obj[i].lastused + obj[i].charges < player.time))) 
	    && (type != 5 || GIFT(i, 'E'))) 
	{
		if (i == player.weapon) 
			sprintf(tmp, "%c] %s", ITOLISTICON(j), NAME(i));
		else if (obj[i].charges == -2) 
			sprintf(tmp, "%c: %s", ITOLISTICON(j), NAME(i));
		else if (obj[i].abilities == (player.drunk + '0') 
		    && player.drunk > 0) 
			sprintf(tmp, "%c> %s", ITOLISTICON(j), NAME(i));
		else sprintf(tmp, "%c) %s", ITOLISTICON(j), NAME(i));
		object[j++]=i;
                composelist(tmp);
	}
        
	if (j>0 && type > 0) 
	{
        	composelist("ESC) none of the above");
	} 
	else if (j == 0) 
	{
		composelist("Nothing Appropriate");
	}
}

int selectunknown(void) {
	int i, j;

	j=0;
	COUNT_DOWN (i, NOBJECTS) 
		if (obj[i].have == 1) 
			object[j++]=i;
	if (j>0) 
		return(rand() % j);
	else
		return(NOBJECTS+1);
}

int getitem(void) {
	char i;
	i=getchar();
	if (i>='0' && i<='9') 
		return(i-'0');
	if (i>='a' && i<='z') 
		return(i-'a'+10);
	if (i>='A' && i<='Z') 
		return(i-'A'+36);
	if (i==27) 
		return(-1);
	return(0);
}

void lose_item(char *thief) {
	int i;

	if (player.objects < 2) 
	    return;
	while (!obj[(i= (rand() % NOBJECTS))].have) 
	    ;
	obj[i].have = 0;
	sprintf(tmp, "%s took your %s", thief, obj[i].name);
	STATE(tmp);
        WAIT();
	player.score -= obj[i].worth;
	if GIFT(i,'a') 
		player.ability[AARMOR]=0;
	if GIFT(i,'T') 
		player.ability[ASEEINVIS]=0;
	if GIFT(i,'w') 
		player.ability[AWILL]=0;
	if GIFT(i,'m') 
		player.ability[AMENTALHEALTH]=0;
	if (GIFT(i,'h') && obj[i].dam > 1) 
		player.ability[AHOLY]=0;
	if GIFT(i,'H') 
		player.ability[ALEARN]=0;
	if (player.drunk == i) 
		player.drunk = 0;
	if (player.weapon == i) 
		player.weapon = HANDS;
        if GIFT(i, 'E')
	{
		int loop_tmp;
		obj[i].charges = -1;
		player.usingsign = 0;
		for (loop_tmp=0; loop_tmp < NUMBERSIGNS; loop_tmp++) 
			if (obj[sign_index[loop_tmp]].charges == -2) 
				player.usingsign = 1;
	}
}

/*
 * Returns the value of the "benefit" selected. This routine places all the
 * available gains of the BENEFIT spell up on the screen and asks the user
 * to pick one of them. It then returns a number corisponding to that gain.
 * If I were a brainiac I would have defined these numbers to some readable
 * tokens.
 */
int get_benefit(void) {
	int i;
        int j = 0;
	int y[20];

	upscreen();
        composelist(NULL);
	if (player.dex<10) 
	{
		y[j++]=1;
		sprintf(tmp, "%d. Restore Dexterity", j);
                composelist(tmp);
	}
	if (player.str<10) 
	{
		y[j++]=2;
		sprintf(tmp, "%d. Restore Strength", j);
                composelist(tmp);
	}
	if (player.intel<10) 
	{
		y[j++]=3;
		sprintf(tmp, "%d. Restore Intelligence", j);
                composelist(tmp);
	}
	if (player.topbody<150) 
	{
		y[j++]=4;
		sprintf(tmp, "%d. Cure Disruption", j);
                composelist(tmp);
	}
	if (player.topsan<150) 
	{
		y[j++]=5;
		sprintf(tmp, "%d. Restore Confidence", j);
                composelist(tmp);
	}
	if (player.body<50) 
	{
		y[j++]=6;
		sprintf(tmp, "%d. Heal Body", j);
                composelist(tmp);
	}
	if (player.sanity<50) 
	{
		y[j++]=7;
		sprintf(tmp, "%d. Restore Sanity", j);
                composelist(tmp);
	}
	if (player.food>0) 
	{
		y[j++]=8;
		sprintf(tmp, "%d. Relieve Hunger", j);
                composelist(tmp);
	}
	i = 0;
	while (i < NPLAGUE) 
	{
		if (plague[i].caught > 0) 
		{
			y[j++]=9;
			sprintf(tmp, "%d. Cure yourself of %s", j, 
                        	plague[i].name);
                        composelist(tmp);
			i = NPLAGUE;
		}
		i++;
	}
	if (j == 0) 
	{
		STATE("Nothing wrong with you...");
		return 0;
	}
	ask("Which cure:");
	i = getitem();
	if (i<1 || i>j) 
		return 0;
	else
		return y[i-1];
}

void touch_sign(char c) {
	if (obj[c].charges == -1 && obj[c].lastused > 0) 
	{
	    obj[c].charges = -2;
	    player.usingsign = 1;
	    sprintf(tmp,"invoking %s", NAME(c));
	    STATE(tmp);
	} 
	else if (obj[c].charges == -1) 
	{
	    sprintf(tmp,"can't activate %s", NAME(c));
	    STATE(tmp);
	    STATE("   it's depleted...");
	} 
	else 
	{
	    int i;
	    sprintf(tmp,"deactivating %s", NAME(c));
	    STATE(tmp);
	    obj[c].charges = -1;
	    player.usingsign = 0;
	    for (i=0; i < NUMBERSIGNS; i++) 
		if (obj[sign_index[i]].charges == -2) 
			player.usingsign = 1;
	}
}

void useobj(void) {
	int i;
	char c, ct;
	
	displayobj(4);
	ask("Which item : ");
	c=getitem();
	if (c>NOBJECTS || c== -1) 
		return;
	c=object[c];
	if (obj[c].have == 0) 
	{
		STATE("you don't have that item."); 
		return;
	}
	if (obj[c].charges == 0) 
	{
		STATE("that object can't be used.");
		return;
	}
	if (obj[c].charges < 0) 
	{
		touch_sign(c);
		return;
	}
	if (obj[c].lastused > 0 
	    && obj[c].lastused+obj[c].charges > player.time) 
	{
		STATE("that object doesn't work right now.");
		return;
	}
	if (obj[c].lastused > 0) 
	{
		obj[c].lastused = player.time;
		obj[c].charges += 50;
	} 
	else 
	{
		obj[c].charges--;
		player.sanity -= obj[c].sanlos;
		if (player.sanity < 1) 
		{
		    sprintf(death,"%s%s", "blew his mind with a ", NAME(c));
		    endgame(0);
		}
	}
	if GIFT(c,'g') 
	{
		if (cty[player.city].lon > 360) 
			Majorsummoning(cty[player.city].lon-361);
		else 
		{
			int m;
			STATE("Ops, summoned up a monster.");
			fight(getmonster(corr(-1) + 100, 0), 0);
			cty[player.city].mon++;
		}
	}
	if GIFT(c,'b') 
		cty[player.city].mon = 0;
	if GIFT(c,'c') 
	{
		i=0;
		while (i < NPLAGUE) 
		{
			if (plague[i].caught > 0) 
			{
			    sprintf(tmp,"%s%s", "You cured yourself of ", 
				plague[i].name);
			    STATE(tmp);
			    plague[i].caught = 0;
			    i = NPLAGUE;
			}
			i++;
		}
	}
	if GIFT(c,'G') 
	{
		if (rand() % player.sanity > 1000) 
		{
			STATE("Your gripped by a maddening disease.");
			player.sanity = (player.sanity * 9) / 11;
			disease(st_pos++);
		} 
		else 
		{
			player.sanity += 100; 
			if (player.sanity > player.topsan) 
				player.topsan += 10;
		}
	}
	if GIFT(c,'B') 
	{
		if (rand() % player.body > 700 
		    || rand() % player.sanity > 700) 
		{
			STATE("Something went wrong.");
			player.sanity -= (MOD_16(rand()) + 20);
			player.body -= (MOD_16(rand()) + 20);
			player.topsan += 10;
			player.topbody += 10;
		}
		else 
		{
			player.sanity += (MOD_16(rand()) + 20);
			player.body += (MOD_16(rand()) + 20);
			player.topsan = (player.topsan * 6) / 5;
			player.topbody = (player.topbody * 6) / 5;
		}
		if (player.str < (rand() % 40 + 16)) 
			player.str++;
		else 
		{
			STATE("Pain racks your body.");
			player.topbody = (player.topbody * 9) / 11;
			player.str--;
		}
		if (player.intel < (rand() % 40 + 16)) 
			player.intel++;
		else 
		{
			STATE("You're plagued with fever dreams.");
			player.topsan = (player.topsan * 9) / 11;
			player.intel--;
		}
		if (player.dex < (rand() % 35 + 11)) 
			player.dex++;
		else 
		{
			disease(st_pos++);
			player.dex--;
		} 
	}
	if GIFT(c,'s') 
	{
		if (player.str < (rand() % 40 + 16)) 
			player.str++;
		else 
		{
			STATE("Pain racks your body.");
			player.topbody = (player.topbody * 9) / 11;
			player.str--;
		}
	}
	if GIFT(c,'i') 
	{
		if (player.intel < (rand() % 40 + 16)) 
		{
			player.intel++;
			player.topsan += 10;
			player.sanity += 5;
		}
		else 
		{
			STATE("You're plagued with fever dreams.");
			player.topsan = (player.topsan * 9) / 11;
			player.intel--;
		}
	}
	if GIFT(c,'d') 
	{
		if (player.dex < (rand() % 35 + 11)) 
			player.dex++;
		else 
		{
			disease(st_pos++);
			player.dex--;
		}
	}
	if GIFT(c,'P') 
	{
		if (player.dex<10) 
			player.dex = (11 + player.dex) / 2;
		if (player.str<10) 
			player.str = (11 + player.str) / 2;
		if (player.intel<10) 
			player.intel = (11 + player.intel) / 2;
		if (player.topbody<150) 
			player.topbody = (151 + player.topbody) / 2;
		if (player.topsan<150) 
			player.topsan = (151 + player.topsan) / 2;
	}
	if GIFT(c,'r') 
	{
		if (rand() % player.body > 750) 
		{
			STATE("You're filled with pain.");
			player.body = (player.body * 9) / 11;
		} 
		else 
		{
			player.body += 50; 
			if (player.body > player.topbody) 
				player.topbody += 10;
		}
	}
	if GIFT(c,'S') 
	{
		if (rand() % player.sanity > 750) 
		{
			STATE("Your gripped by intense fear.");
			player.sanity = (player.sanity * 9) / 11;
		} 
		else 
		{
			player.sanity += 50; 
			if (player.sanity > player.topsan) 
				player.topsan += 10;
		}
	}
	if GIFT(c,'t') 
	{
		ct = list(180, 1);
		if (ct == 0 || ct > pc) 
			return;
		else 
			player.city = cp[ct - 1];
	}
	if GIFT(c,'C') 
	{
		ct = list(180, 1);
		if (ct == 0 || ct > pc) 
			return;
		else 
			ct--;
		COUNT_DOWN ( i, NCITIES) 
		{
			cty[i].level += rand() % 300;
			if (i==cp[ct] && corr(i) < 15000) 
				cty[i].level = 
					(corr(i) * 2) - player.total_corruption;
		}
		fillscreen();
	}
	if GIFT(c,'D') 
	{
		ct = list(180, 2);
		if (ct == 0 || ct > pc) 
			return;
		else 
			player.city = cp[ct - 1];
		fillscreen();
	}
	if GIFT(c,'k') 
		addspell();
	displayobj(4);
}

/*
 * Handle having the player drink from one of the cups. Set player.drunk
 * element to equal which cup he/she has drunk from.
 */
void drink(void) {
	char c;
	int i;
	
	displayobj(2);
	ask("Which item : ");
	c=getitem();
	if (c>NOBJECTS || c== -1) 
		return;
	if (obj[object[c]].have == 0) 
	{
		STATE("you don't have that item."); 
		return;
	}
	c=obj[object[c]].abilities;
	if (c < '0' || c > '9') 
	{
		STATE("That's a funny thing to drink...");
		return;
	}
	player.drunk = c - '0';
	if (c=='0') 
		player.drunk = 10;
	COUNT_DOWN ( i, NCITIES )
	    if (cty[i].lon == 360 + player.drunk) 
	    {
		    sprintf(tmp, "%s%s", "you are safe in ", cty[i].name);
		    STATE(tmp);
	    }
}

/*
 * This function sets the internal variable player.weapon to point to which
 * object the player is using to attack with. An invalid answer from getitem()
 * will set the object to zero (bare hands).
 */
void wield(void) {
	char c;
	
	displayobj(1);
	ask("Which item : ");
	c=getitem();
	if (c>NOBJECTS || c== -1) 
		return;
	if (obj[object[c]].have == 0) 
	{
		STATE("Now wielding your hands.");
		player.weapon = HANDS;
		displayobj(1);
		return;
	}
	player.weapon = object[c];
	sprintf(tmp, "%s%s", "Now wielding ", NAME(player.weapon));
	STATE(tmp);
}

/*
 * This routine governs whether a player can find out what the object he/she
 * has really is. The more powerful an object the harder it is to identify.
 * A brainiac would set some objects more relatively easy to identify than
 * others (for instance, it takes a genius to identify a shotgun now). Some
 * artifacts aid your ability to identify things.
 */
int lookup(int flag) {
	char c;
	
	if (flag==0) 
	{
		displayobj(3);
		ask("Which item : ");
		c=getitem();
		if (c>NOBJECTS || c== -1) 
			return(1);
		if (obj[object[c]].have == 0) 
		{
			STATE("you don't have that item."); 
			return(0);
		}
		if (obj[object[c]].have == 2) 
		{
			STATE("you already know what that is."); 
			return(0);
		}
	} 
	else 
	{
		c=selectunknown();
		if (c>NOBJECTS) return(2);
	}
	if (((player.ability[ALEARN] * 50) + player.intel - 
	    (obj[object[c]].worth/50) - (rand() % 50))<0) 
	{
		if (flag==0) 
		{
			STATE("Sorry, that one stumps you."); 
		}
		return(0); 
	}
	if (obj[object[c]].have < 2) 
		player.score += (obj[object[c]].worth / 2);
	obj[object[c]].have=2;
	if (flag==0) 
	{
		sprintf(tmp, "You find out it's %s", NAME(object[c]));
		STATE(tmp);
	} else {
        	sprintf(tmp, "You've indentified %s", NAME(object[c]));
        	composelist(tmp);
        }
	return(1); 
}

/*
 * This routine looks for objects in the current town of an appropriate
 * power level. The level is indicated by the variable of the name
 * 'level'. The higher the value, the less important the object can be.
 * Two internal variables (min and max) are set to a range of all the
 * objects that can be discovered in that power range. An object is then
 * selected in that range. If the player already has it and it's fully
 * charged, then the routine exits. Otherwise it supplies the player with
 * the object, and any powers granted by the object.
 */
int did_find(int level) {
	int obj_found, max= -1, min= -1, town_corr = corr(-1);
	int i;
	char temp[MAXPATH];

#ifndef DEBUG
	if (rand() % 30 > player.intel)
		return(0);
#endif

	COUNT_DOWN ( i, NOBJECTS) 
	{
		int obj_worth = obj[i].worth;

		if (max == -1 && obj_worth < town_corr + 100)
			max = i;
		if (obj_worth > town_corr - 200 * level)
			min = i;
	}

	if (min == -1) 
		min == 0;
	if (max == -1)
		max = NOBJECTS - 1;

	do {
		struct object_table *o;
		if (max - min == 0)
			obj_found = min;
		else
			obj_found = min + (rand() % (max - min)); 

		if (GIFT(obj_found, '$'))
			break;

		o = &obj[obj_found];
		if (o->have > 0)
		{
			if (o->lastused == 0)
			{
				if (o->charges > -1 &&
				    (rand() % (1 + o->charges) != 0))
                                {
					obj_found = -1;
                                        if (min > 0)
                                        	min--;
                                        else if (max < NOBJECTS - 1)
                                        	max++;
                                }
			}
			else
				return(0);
		}
	} while (obj_found == -1);

	cty[player.city].tres--;

	if (GIFT(obj_found, '$')) 
	{
		STATE("You found some hidden money.");
		player.money += (rand() % (69 + town_corr / 2) + 100);
		return(1);
	}

        if (obj[obj_found].have == 0)
        	obj[obj_found].have = 1;
	if (obj[obj_found].lastused == 0) 
	{
		sprintf(temp, "You found some more pieces of %s.", 
			NAME(obj_found));
		STATE(temp);
		player.score += (obj[obj_found].worth / 5);
		player.sanity -= (obj[obj_found].sanlos /2);
		if (obj[obj_found].charges > -1) 
			obj[obj_found].charges += (1 + MOD_2(rand()));
		else 
			obj[obj_found].lastused += (50 + rand() % 100);
		return(1);
	}

	sprintf(temp, "You found %s.", NAME(obj_found));
	STATE(temp);
	player.objects++;
	player.score += (obj[obj_found].worth / 2);
	if (obj[obj_found].lastused > 0) 
		player.sanity -= obj[obj_found].sanlos;
	if (obj[obj_found].charges == 0) 
	{
		if GIFT(obj_found,'a') 
			player.ability[AARMOR]=1;
		if GIFT(obj_found,'T') 
			player.ability[ASEEINVIS]=1;
		if GIFT(obj_found,'w') 
			player.ability[AWILL]=1;
		if GIFT(obj_found,'m') 
			player.ability[AMENTALHEALTH]=1;
		if (GIFT(obj_found,'h') && obj[obj_found].dam > 1) 
			player.ability[AHOLY]=1;
		if GIFT(obj_found,'H') 
			player.ability[ALEARN]=1;
	}
	return(1);
}

/*
 * Function to search the current city for objects. Note that if the variable
 * "power" is set to non-zero, then the routine will always find something
 * in the city if there is anything there. This function keeps calling
 * did_find() with increasingly higher numbers, which means the did_find()
 * function will look for less and less valuable objects in the city. When
 * power is set to zero (normal) then the routine will only search as many
 * times as the player's intelligence will allow (Int 1-4 once, 5-9 twice,
 * 10-14 thrice, 15-19 four time, etc...).
 */
void search(int rec, int power) { // power = 0 is normal, power = 1 is always find
	int level = 0;

	if (cty[player.city].tres == 0) 
	{
		STATE("You couldn't find anything.");
		return;
	}

	if (power != 0)
	{
		while (did_find(level) == 0)
			level++;
	} 
	else
	{
		while (did_find(level) == 0)
		{
			if (level > (2 + player.intel / 5)) 
				rec--;
			if (rec < 0)
			{
				STATE("You couldn't find anything.");
				return;
			}
			level++;
		}
	}
}

/*
 * Some monsters have a pipe. When a player fights someone who has a pipe, a
 * random pipe is given to him/her if they don't already have it.
 */
void getpipe(void) {
	int i;

	for (i=0; i<NOBJECTS; i++)
		if (strcmp(PIPE, obj[i].generic) == 0 && obj[i].have == 0)
			break;
	if (i == NOBJECTS)
		return;
	obj[i].have++;
	player.objects++;
	player.score += (obj[i].worth / 2);
	STATE("You find a pipe on the servitor.");
}

/*
 * Some monsters have an axe (actually, only the madman). When a player
 * fights someone who has an axe, the axe is given to him/her if they don't
 * already have one.
 */
void getaxe(void) {
	int i;

	for (i=0; i<NOBJECTS; i++)
		if (strcmp(AXE, obj[i].generic) == 0)
			break;
	if (obj[i].have>0) 
		return;
	obj[i].have++;
	player.objects++;
	player.score += (obj[i].worth / 2);
	STATE("You get an axe off the madman.");
}

/*
 * Some corpses' have money on them (priests and the like). Give the user
 * some money based on the corpse/monster's power level.
 */
void robcorpse(int m) {
	STATE("You find some money on the corpse.");
	player.money += rand() % mon[m].level;
}
