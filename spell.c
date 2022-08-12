#include "spell.h"
#include "cthulhu.h"
#include "init.h"
#include "misc.h"
#include "graphics.h"
#include "travel.h"
#include "objects.h"
#include "clock.h"
#include "heal.h"
#include "monsters.h"

/*
 * Subroutine for the mythos ritual. This tells the user something about the
 * game, namely what monsters belong in the CLASS of monsters that can do
 * a certain thing (like be invisible).
 */
void mythos(void) {
	register int i, j;
	int and;
	static struct {
		char poop;
		char *scoop;
	} bloop[26] = {
	    'A', "generate a death Aura",
	    'W', "are able to spin webs",
	    'c', "are able to corrupt",
	    'r', "are magic resistance",
	    'D', "disrupts those who touch it",
	    'I', "are invisible",
	    'u', "are undead",
	    'i', "are only affected by spells",
	    'w', "you can get a axe off",
	    'v', "are vampiric",
	    'P', "travel in packs",
	    'V', "are venomous",
	    'C', "can carry away a victim",
	    '@', "carry a pipe",
	    'a', "are armored",
	    's', "can summon allies",
	    't', "can steal items",
	    '$', "have money",                          
	    'p', "are poisonous",
	    'R', "regenerate",
	    'S', "are susceptible to light",
	    'h', "can hypnotize",
	    'H', "can howl",
	    'G', "are Headmasters",
	    'E', "can affects protected people",
	    'd', "carry diseases"};

        composelist(NULL);
	and=rand() % 24;
	sprintf(tmp, "The following creatures %s:", bloop[and].scoop);
        composelist(tmp);
	COUNT_DOWN (i, NMONSTERS)
		if (POWER(i, bloop[and].poop)) 
		{
                	composelist(mon[i].name);
                }
	WAIT();
}

/*
 * All the possible spells that can be cast out of combat (rituals) are figured
 * out here. Some of them depend on sub functions.
 */
void castspell(void) {
	register int i;
	char c, ct;
	char displayspells();
	char *dclevel();
	
	c=displayspells(0);
	switch(c) 
	{
	case IDENTIFY:
		displayobj(3);
		ask("Which item : ");
		ct=getitem();
		if (ct>NOBJECTS || c== -1) 
			return;
		if (obj[object[ct]].have == 0) 
		{
			STATE("you don't have that item."); 
			return;
		}
		if (obj[object[ct]].have < 2) 
			player.score += (obj[object[ct]].worth / 2);
		obj[object[ct]].have++;
                sprintf(tmp, "You find out it's %s", NAME(object[ct]));
		STATE(tmp);
		break;
	case FINDING:
		search(0, 1);
		break;
	case FOOD:
		player.food = (player.food < 50) ? 0 : (player.food - 50);
		break;
	case ISOLATION:
		if (player.total_corruption < 10000) 
		    player.total_corruption = (player.total_corruption * 3)/4;
		else 
		    player.total_corruption = (player.total_corruption * 4)/5;
		COUNT_DOWN ( i, NCITIES) 
		    if (i != player.city && corr(i) < 15000) 
			cty[i].level += rand() % 300;
		break;
	case SACRIFICE:
		sprintf(death,"%s","used up in sacrifice spell");
		i = rand() % 17;
		switch (i) 
		{
		    case 0: player.food += 50; break;
		    case 1: player.body /= 2; break;
		    case 2: player.body -= 75; break;
		    case 3: player.topbody /= 2; break;
		    case 4: player.topbody -= 100; break;
		    case 5: player.sanity /= 2; break;
		    case 6: player.sanity -= 75; break;
		    case 7: player.topsan /= 2; break;
		    case 8: player.topsan -= 100; break;
		    case 9: player.dex /= 2; break;
		    case 10: player.dex -= 10; break;
		    case 11: player.str /= 2; break;
		    case 12: player.str -= 10; break;
		    case 13: player.intel /= 2; break;
		    case 14: player.intel -= 10; break;
		    case 15: lose_item("sacrifice spell"); break;
		    case 16: disease(st_pos++); disease(st_pos++); break;
		    default: STATE("Huh? nothing-bad-happened bug"); break;
		}
		i = get_benefit();
		switch (i) 
		{
		    case 0: break;
		    case 1: player.dex = (11 + player.dex) / 2; break; 
		    case 2: player.str = (11 + player.str) / 2; break; 
		    case 3: player.intel = (11 + player.intel) / 2; break; 
		    case 4: player.topbody = (150 + player.topbody) / 2; break; 
		    case 5: player.topsan = (150 + player.topsan) / 2; break; 
		    case 6: player.body = (150 + player.body) / 2; break; 
		    case 7: player.sanity = (150 + player.sanity) / 2; break; 
		    case 8: player.food = (player.food>5)?(player.food/2):0;
			    break;
		    case 9: c = 0;
			    while (c < NPLAGUE) 
			    {
				if (plague[c].caught > 0) 
				{
				    plague[c].caught = 0;
				    c = NPLAGUE;
				}
				c++;
			    }
			    break;
		    default: STATE("Choose funny option bug"); break;
		}
		break;
	case MYTHOS:
		mythos();
		break;
	case HEALING:
		if (corr(-1) > 4000) 
		{
			STATE("For some reason it's not as affective.");
			player.body += (rand() % 10 + 5);
			player.topbody -= (rand() % 5 + 3);
		} 
		else 
		{
			player.body += (rand() % 15 + 8);
			STATE("that feels good.");
		}
		if (player.feddisease < 19) 
			player.feddisease++;
		CthulhuClock();
		break;
	case SOLITUDE:
		burntime(125 + rand() % 75);
		STATE("nothing like a nice long rest...");
		COUNT_DOWN (i, 100)
		    if (rand() % player.topsan > player.sanity) 
		    {
			    player.topsan++;
			    player.sanity++;
		    }
		CthulhuClock();
		break;
	case LONGEVITY:
		STATE("your body feels more at ease...");
		player.topsan += (rand() % 10);
		player.topbody += (rand() % 10);
		player.longevity = 1;
                display_plagues("You've diagnosed yourself as having:");
		break;
	case DISCOVER: 
	    {
		int downlines=0;
		clear();
		COUNT_DOWN ( i, NCITIES ) 
		{
		    move(downlines, MOD_2(i) * 40);
		    printw("%-18.18s%3d %-17.17s", 
			cty[i].name, 
			cty[i].mon,
			(cty[i].lon > 360) ? 
			    ((goal & (1 << (cty[i].lon - 361)))?
				    " <banished>":headmaster[cty[i].lon-361]) : 
				mon[getmonster(corr(i), 0)].name);
		    downlines += MOD_2(i);
		    if (downlines > LINES - 4) 
		    {
			downlines = 0;
			WAIT();
			clear();
		    }
		}
		WAIT();
		clear();
		fillscreen();
		break;
	    }
	case SANCTUARY:
		ct=0;
		STATE("\"There's no place like home.\"");
		COUNT_DOWN ( i, NCITIES ) 
			if (corr(i) < corr(ct)) 
				ct=i;
		player.city = ct;
		break;
	case REGENERATION:
		player.ability[AREGEN] = 1;
		STATE("You can feel yourself improving");
		break;
	case STEALTH:
		player.stealth = 1;
		STATE("you're now invisible,");
		STATE("  but you don't know for how long.");
		break;
	case TELEPORT:
		ct = list(180, 1);
		if (ct == 0 || ct > pc) 
			return;
		else 
			player.city = cp[ct - 1];
		break;
	case RETURN:
		STATE("I'm outta' here");
		do ct = rand() % NCITIES;
		while (cty[ct].lon > 360) ;
		player.city = ct;
		break;
	default: 
		break;
	}
}

/*
 * Display all the spells the user knows and then return the "ability" of
 * that spell to the calling routine. If the user aborts the spell, return
 * a blank space ' '.
 */
char displayspells(int flag) {
	register int i, j;
	char s[NSPELLS];

        composelist(NULL);
	j = 0;
	COUNT_DOWN ( i, NSPELLS ) 
	{
		if (spell[i].know > 0 && flag == spell[i].cbt) 
		{
			s[j]=i;
			sprintf(tmp, "%c) %s", ITOLISTICON(j), spell[i].name);
                        composelist(tmp);
			j++;
		}
	}
	if (j>0) 
	{
		composelist("ESC) No Spell");
		ask("What spell : ");
		i=getitem();
		composelist(NULL);
	} 
	if (i < 0 || j == 0 || i + 1 > j) 
		return(' ');
	player.sanity -= spell[s[i]].sanlos;
	cty[player.city].level += spell[s[i]].sanlos;
	return(spell[s[i]].ability);
}

/*
 * Find a new spell and mark the spell known array as having known it.
 */
void addspell(void) {
	register int i;

	if (player.spells==NSPELLS) 
	{
		STATE("You already know all the spells there are.");
		return;
	}
	do 
        	i=rand() % NSPELLS;
	while (spell[i].know !=0) ;
	player.spells++;
	spell[i].know++;
	sprintf(tmp,"You learned the %s",spell[i].name);
	STATE(tmp);
}
