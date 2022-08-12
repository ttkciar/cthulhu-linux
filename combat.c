#include "cthulhu.h"
#include "combat.h"
#include "misc.h"
#include "init.h"
#include "monsters.h"
#include "graphics.h"
#include "objects.h"
#include "spell.h"
#include "end_game.h"

void cant_avoid_combat(int x) {
	fight(getmonster(corr(-1), 0), 0);
}

void init_fight(int m, int recur) {
	if (recur == 0) 
	{
		WAIT();
		clear();
	}
	if (ft_stat.lastcbt != player.time) 
	{
		ft_stat.excel = 0;
		ft_stat.escape = 0;
		ft_stat.invisible = 0;
		ft_stat.immune = 0;
		ft_stat.protected = 0;
		ft_stat.clensing = 0;
		ft_stat.rage = 0;
		ft_stat.webs = 0;
		ft_stat.lastcbt = player.time;
	}
	signal(2, cant_avoid_combat);
	if (ft_stat.escape != 0) 
	{
		if (POWER(m, 'C') || POWER(m, 'G')) 
		{
			STATE("Your spell wasn't powerful enough.");
			ft_stat.escape = 0;
		} 
		else 
			return;
	}
	if (recur == 0) 
	{
		if (mon[m].san > 0) 
                	player.sanity -= (rand() % (int) mon[m].san);
		if (player.ability[AMENTALHEALTH] == 0 
                    && rand() % 100 > player.sanity)
			player.sanity -= mon[m].san;
	}
	if POWER(m, 'P') 
	{
		if (recur == 0) 
		{
			STATE("These monsters come in packs.");
			fight(m, (rand() % 5) + 1);
		} 
		else if (recur > 1) 
			fight(m, recur-1);
	}
	if POWER(m, 's') 
	{
		int y;
		y = rand() % 5;
		while (y-- > 0) 
		{
			sprintf(tmp, "%s%s", mon[m].name, 
			    " has you fight another first.");
			STATE(tmp);
			fight(getmonster(corr(-1) + 500, 1),0);
			clear();
		}
	}
	if (POWER(m, 'C') && rand() % 25 > player.dex) 
	{
		player.city = rand() % NCITIES;
		sprintf(tmp, "The %s carries you off to %s", mon[m].name, 
			cty[player.city].name);
		STATE(tmp);
	}
	if (POWER(m, 't') && ((int)(rand() % 35) < (int)(mon[m].dex)))
		lose_item(mon[m].name);
	mpd = mon[m].body;
}

void header_fight(int m) {
	move(0, 0); clrtoeol();
	printw("%38s%4d%13s%4d",
	    "Hit Points : ", player.body, "sanity : ", player.sanity);
	move(1,0); clrtoeol();
	if (mpd > 0) 
		printw("%-25.25s%13s%4d", mon[m].name, "Hit Points : ", mpd);
	else 
		printw("%-25.25s%17s", mon[m].name, "Hit Points : DEAD");
	move(2,0); clrtoeol();
	if (ft_stat.protected > 0) 
		addstr("Protected ");
	if (ft_stat.immune > 0) 
		addstr("Immune ");
	if (ft_stat.holyaura > 0) 
		addstr("Holy Aura ");
	if (ft_stat.clensing > 0) 
		addstr("Cleansing ");
	if (ft_stat.rage > 0) 
		addstr("Berserker Rage ");
	if (ft_stat.excel > 0) 
		addstr("Excelled ");
	if (ft_stat.invisible > 0) 
		addstr("Invisible ");
        move(3, 0); clrtoeol();
}

void fight(int m, int recur) {
	char mtdex, hdex, mdex;
	int y, t;
	char c;
	
	init_fight(m, recur);
	header_fight(m);
	mtdex = mon[m].dex;
	ft_stat.holyaura = 0;

	while (player.body>0 && mpd>0 && player.sanity>0) 
	{
	    y=4; /* Set up current line to print on */

	    if (POWER(m, 'R') && mpd < (int)(mon[m].body)) 
	    {
		    mpd += mpd / 10;
		    if (mpd > (int)(mon[m].body))
                        mpd = mon[m].body;
	    }
	    if POWER(m, 'c') 
	    {
		    if ((player.ability[AHOLY]>0 && rand() % 3 == 1) 
			|| (ft_stat.immune > 0 && MOD_2(rand()) == 1)) 
			    ;
		    else 
		    {
			    move(y++, 1); clrtoeol();
			    addstr("\nIt casts a corruption spell on you\n");
			    player.dex -= rand() % 3;
			    player.str -= rand() % 3;
			    player.intel -= rand() % 3;
			    player.body = (player.body * 9) / 11;
			    player.sanity = (player.sanity * 9) / 11;
		    }
	    }
	    if (POWER(m, 'A'))
	    {
		if (((player.ability[AHOLY] > 0)?1:0) 
		    + ((ft_stat.immune > 0)?1:0)
		    + ((ft_stat.protected > 0)?1:0) < 2)
		{
			move(y++, 1); clrtoeol();
			addstr("\nYou're affected by it's presence\n");
			player.body = (player.body * 8) / 11;
			player.sanity = (player.sanity * 8) / 11;
		}
	    }
	    hdex = player.dex + ft_stat.excel;
	    if (POWER(m, 'W'))
	    {
		    ft_stat.webs++;
		    hdex = (hdex > ft_stat.webs) ? (hdex - ft_stat.webs) : 0;
		    move(y++, 1); clrtoeol();
		    addstr("It's webs slow you down.");
	    }

	    ask("combat: ");
	    c = '0';
#ifdef SYSV
	    while (strchr(" cew?hat", c) == NULL) 
		c=getchar();
#else
	    while (index(" cew?hat", c) == NULL) 
		c=getchar();
#endif
	    if (c == '\?' || c == 'h') 
	    {
		    STATE("<SPACE> - fight  t)ouch elder sign");
		    STATE("c)ast spells     w)ield a different weapon");
		    STATE("a)void combat    e)scape");
		    ask("combat: ");
#ifdef SYSV
		    while (strchr(" cewat", c) == NULL) 
			c=getchar();
#else
		    while (index(" cewat", c) == NULL) 
			c=getchar();
#endif
	    }
	    if (c == 't') 
	    {
		    displayobj(5);
		    ask("Which Elder Sign : ");
		    c=getitem();
		    if (c != -1) 
		    {
			    c=object[c];
			    if (obj[c].have == 0) 
			    {
				    STATE("you don't have that sign."); 
			    }
			    else 
				    touch_sign(c);
		    }
		    c = 't';
	    }
	    if (c == 'a') 
		    hdex = 0;
	    if (c=='w') 
	    {
		    wield();
		    hdex -= 4;
	    }
	    if (c=='c') 
	    {
		    hdex -= 7;
		    if (player.spells == 0) 
		    {
		       STATE("You have no spells.");
		    } 
		    else 
		    {
			c=displayspells(1);
			switch (c) 
			{
			case ESCAPE:
			    if (rand()%10 
				&& !POWER(m,'C') 
				&& !POWER(m,'G')) 
			    {
				STATE("you escaped.");
				WAIT();
				if (recur == 0) 
				{
					clear();
					fillscreen();
					signal(2, Quit); 
					/*  turn on ^q to quit */
				} 
				else 
					ft_stat.escape = 1;
				return;
			    } 
			    else 
				STATE("you failed to get away.");
			    break;
			case IMMUNITY: 
			    ft_stat.immune = 1; 
			    STATE("You're immune now.");
			    break;
			case BERSERKER: 
			    ft_stat.rage = 1;
			    STATE("You're gripped by a berserker rage.");
			    break;
			case HOLY_AURA: 
			    ft_stat.holyaura = 1;
			    STATE("An aura surrounds you.");
			    break;
			case INVISIBILITY:
			    ft_stat.invisible = 1;
			    STATE("You grow invisible.");
			    break;
			case EXCEL:
			    if (ft_stat.excel == 0) 
			    {
				    ft_stat.excel = 6;
				    player.food += 2;
				    STATE("You feel quickened.");
			    } 
			    else 
			    {
				    ft_stat.excel += 5;
				    player.food += 5;
				    STATE("You feel even faster,");
				    STATE("   but something's wrong.");
				    y += 2;
				    disease(y);
			    }
			    break;
			case CLEANSING: 
			    ft_stat.clensing = 1;
			    STATE("The grounds burst with clensing flame.");
			    break;
			case PROTECTION: 
			    if POWER(m, 'E') 
			    {
				    STATE("You spell fades");
				    STATE("as soon as the monster attacks");
			    } 
			    else 
			    {
				    ft_stat.protected = 1; 
				    STATE("You're safer now.");
			    }
			    break;
			case VAMPIRE:
			    if (!POWER(m,'r') || rand() % 5 == 1) 
			    {
				    STATE("You suck the life force out of it.");
				    mpd -= (MOD_16(rand()) + 5);
				    player.body += (MOD_8(rand()) + 5);
				    if (player.food > 4) 
					player.food -= 5;
				    else 
					player.food = 0;
			    } 
			    else 
			    {
				    STATE("It was resistant to the attack.");
			    }
			    break;
			case WITHER:
			    if (!POWER(m,'r') || rand() % 5 == 1) 
			    {
				    STATE("It begins to wither.");
				    mtdex -= (3 + rand() % 5);
			    } 
			    else 
			    {
				    STATE("It was resistant to the attack.");
			    }
			    break;
			case CORRUPT:
			    if (!POWER(m,'r') || rand() % 5 == 1) 
			    {
				    STATE("It's corrupted.");
				    mdex = mtdex;
				    while (mdex-- > 0) 
					mpd -= (rand() % 10); 
			    } 
			    else 
			    {
				    STATE("It was resistant to the attack.");
			    }
			    break;
			case BLAST:
			    if (!POWER(m,'r') || rand() % 5 == 1) 
			    {
				    STATE("You blast it.");
				    mpd -= (20 + rand() % 81);
				    if POWER(m,'S') mpd -= 50;
			    } 
			    else 
			    {
				    STATE("It was resistant to the attack.");
			    }
			    break;
			case JUDGEMENT:
			    if (!POWER(m,'r') || MOD_2(rand()) == 1) 
			    {
				    STATE("Awesome forces affect both of you.");
				    while (mpd > 0 && player.body > 0) 
				    {
					if (mpd > player.body)
					    player.body -= (rand() % 9);
					else 
					    player.body -= (rand() % 12);
					mpd -= (5 + rand() % 12);
				    }
			    } 
			    else 
			    {
				    STATE("It was resistant to the attack.");
			    }
			    break;
			default: 
			    hdex += 7;
			    break;
			}
			WAIT();
		    }
		    c = 'c';
	    }
	    if (c=='e') 
	    {
		if (player.dex > (int)(mon[m].dex - (int)(5 + rand() % 11))) 
		{
			if (recur == 0) 
			{
				STATE("you escaped.");
				WAIT();
				clear();
				fillscreen();
				signal(2, Quit); /* turn on ^q to quit */
			} else {
				STATE("you escaped from one of them.");
				WAIT();
			}
			return;
		} else {
			STATE("you failed to get away.");
                        WAIT();
			hdex -= 6;
		}
	    }
	    mdex = mtdex;
	    move(y,1); 
            /* clrtobot(); */
	    if (ft_stat.clensing == 1) 
	    {
		    mpd = mpd - (rand() % player.intel + 1);
		    move(y++, 1); clrtoeol();
		    addstr("It's affected by the clensing field.");
	    }
	    if (POWER(m,'p') 
		&& !ft_stat.immune 
		&& !ft_stat.protected 
		&& (rand() % 40) < player.dex) 
	    {
		    player.dex--;
		    move(y++, 1); clrtoeol();
		    addstr("You are affected by its poison.  ");
	    }
	    if (POWER(m,'d') 
		&& (rand() % 30) < ( 3 - ft_stat.immune - ft_stat.protected)) 
			disease(y++);
	    if (POWER(m,'V') && !ft_stat.immune) 
	    { 
		    if (rand() % 50 < player.dex) 
			player.dex--;
		    if (rand() % 50 < player.intel) 
			player.intel--;
		    if (rand() % 50 < player.str) 
			player.str--;
		    player.topsan -= 5;
		    player.topbody -= 5;
		    move(y++, 1); clrtoeol();
		    addstr("You are affected by its venom.  ");
	    }
	    if (POWER(m, 'h') || POWER(m, 'H')) 
	    {
		if (player.ability[AWILL]==0) 
		{
		    hdex = rand() % hdex;
		    move(y++, 1); clrtoeol();
                    if (POWER(m, 'H'))
                    	printw("Its' howling causes you to stumble.");
                    else
		        printw("Its' hypnosis causes you to faulter.");
		    if POWER(m, 'H') 
		    {
			    player.sanity -= 10;
			    player.topsan -= 10;
		    }
		} 
		else if (!POWER(m, 'G')) 
		{
		    if (POWER(m, 'h')) 
		    {
			move(y++, 1); clrtoeol();
			addstr("you frighten it");
			mdex = rand() % mdex;
		    } 
		    else 
		    {
			move(y++, 1); clrtoeol();
			addstr("you feel an inner strength from your tattoo");
			player.sanity++;
			player.topsan += 2;
		    }
		}
	    }
	    while (hdex>0 || mdex >0) 
		if (hdex>mdex && player.body>0 && mpd>0) 
		{
		    hdex -= 5; 
		    if (POWER(m,'I') 
			&& player.ability[ASEEINVIS]==0 
			&& rand() % 50 > player.dex) 
		    {
			    move(y++, 1); clrtoeol();
			    addstr("its invisibility causes you to miss.");
		    } 
		    else 
		    {
			t = Hattack(m);
			if (ft_stat.rage > 0) 
			{
			      player.body -= (t / 4);
			      t = t * 2;
			}
			mpd -= t;
			move(y++, 1); clrtoeol();
			printw("You do %d damage",t);
			if (POWER(m,'D') 
			    && !ft_stat.immune 
			    && (player.weapon == HANDS 
				|| !GIFT(player.weapon, 'n'))) 
			{
				 player.topbody -= 
				     (Mattack(m) / (1 + ft_stat.protected));
				 player.food += 5;
				 move(y++, 1); clrtoeol();
				 addstr("   Your body is disrupted.");
			}
			if (mpd<1) 
			{
				STATE("it's dead");
			}
		    }
		} 
		else if (player.body>0 && mpd>0) 
		{
		    mdex -= 5;
		    move(y++, 1); clrtoeol();
		    if (ft_stat.invisible == 1 
			&& rand() % 40 > mtdex 
			&& !POWER(m,'r')
			&& !POWER(m,'I') 
			&& !POWER(m,'S')) 
		    {
			    addstr("Your invisiblity saves you.");
		    } 
		    else 
		    {
			    if (ft_stat.invisible == 1) 
			    {
				addstr("It sees you and ");
			    }
			    t = Mattack(m) / (1 + ft_stat.protected); 
			    if (player.usingsign == 1) 
			    {
				int tm;
				tm = saved_by_sign(m, t);
				if (tm > 0) 
				{
				    t = 0;
				    addstr("your elder sign saved you");
				    if (tm == 2) 
					addstr(" but ran out of power");
				    move(y++, 1); clrtoeol();
				}
			    }
			    player.body -= t;
			    printw("It does %d damage", t);
			    if (POWER(m,'v') && t > 1) 
			    {
				mpd = mpd + (t / 2);
				player.food += (t / 4);
				addstr(", feasting on your blood");
			    }
			    if (ft_stat.holyaura == 1) 
			    {
				mpd = mpd - (2 * (rand() % 10 + 1));
		addstr(", but burns at the touch of your holy aura");
			    }
			}
	    } 
	    else 
	    {
		    hdex -= 5; 
		    mdex -= 5;
	    }
	    header_fight(m);
	}
	if (mpd<1) 
	{
		if (recur == 0) 
		{
			player.score += mon[m].level;
			cty[player.city].mon--;
		}
		if POWER(m,'$') 
			robcorpse(m);
		if POWER(m,'w') 
			getaxe();
		if POWER(m,'@') 
			getpipe();
	} 
	else 
	{
        	if (player.body > 0)
                	sprintf(death, 
				"driven insane by sight of %s", mon[m].name);
                else
			sprintf(death, "killed by %s", mon[m].name);
		endgame(0);
	}
	if (recur == 0) 
		WAIT();
	clear();
	if (recur == 0) 
	{
		fillscreen();
		signal(2, Quit);
	}
	fflush(stdin);
	return;
}

int saved_by_sign(int m, int damage) {
	int itmp = -1;

	if (obj[sign_index[0]].charges == -2 		/* poison or venom */
	    && (POWER(m,'p') || POWER(m,'v'))
	    && obj[sign_index[0]].have>0) 
		itmp=0;
	else if (obj[sign_index[1]].charges == -2 	/* armor */
	    && POWER(m,'a') 
	    && obj[sign_index[1]].have > 0) 
		itmp=1;
	else if (obj[sign_index[2]].charges == -2 	/* carry away */
	    && POWER(m,'C') 
	    && obj[sign_index[2]].have > 0) 
		itmp=2;
	else if (obj[sign_index[3]].charges == -2 	/* howling or hypno */
	    && (POWER(m,'h') || POWER(m,'H')) 
	    && obj[sign_index[3]].have>0) 
		itmp=3;
	else if (obj[sign_index[4]].charges == -2 	/* invisible */
	    && POWER(m,'i') 
	    && obj[sign_index[4]].have > 0) 
		itmp=4;
	else if (obj[sign_index[5]].charges == -2 	/* Summoners */
	    && POWER(m,'s') 
	    && obj[sign_index[5]].have > 0) 
		itmp=5;
	else if (obj[sign_index[6]].charges == -2 	/* Gods */
	    && POWER(m,'G') 
	    && obj[sign_index[6]].have > 0) 
		itmp=6;
	if (itmp < 0) 
		return(0);
	obj[sign_index[itmp]].lastused -= damage;
	if (obj[sign_index[itmp]].lastused < 1) 
	{
		obj[sign_index[itmp]].lastused = 0;
		obj[sign_index[itmp]].charges = -1;
		player.usingsign=0; 
		for (itmp=0; itmp < NUMBERSIGNS; itmp++) 
		    if (obj[sign_index[itmp]].charges == -2) 
			player.usingsign=1;
		return(2);
	} 
	return(1);
}

int Mattack(int m) {
	char dice, amount = 0, sides;

	dice = mon[m].dice; 
	sides = mon[m].dam - ((player.ability[AARMOR]>0)?2:0);
	if (sides < 1) 
		sides = 1;
	while (dice-- > 0) 
		amount += (rand() % sides) +1;
	return(amount);
}

int Hattack(int m) {
	char dice, dam, amount = 0;
	char dstr, darmor, dweapon=1;

	dstr = player.str / 5;
	darmor = POWER(m, 'a')?-2:0;
	if (player.weapon != HANDS) 
	{
		if GIFT(player.weapon, 'n') 
			dstr = 0;
		dweapon = obj[player.weapon].dam;
	}
	dam = dstr + darmor + dweapon;
	if (dam < 1) 
		dam = 1;
	dice = (player.weapon != HANDS) ? obj[player.weapon].dice : 1;
	while (dice-- > 0) 
		amount += (rand() % dam) + 1;
	if (player.weapon > -1 && GIFT(player.weapon,'v')) 
	{
		player.body += 3;
		if (player.food > 0) 
		{
			player.food-- ;
			player.sanity-- ;
		}
		player.sanity -= 2;
	}
	if POWER(m,'u') 
	{
		if (player.ability[AHOLY]==0) 
			amount /= 2;
		else if (player.weapon > -1 && !GIFT(player.weapon,'h')) 
			amount /= 2;
		if (player.weapon > -1 && GIFT(player.weapon,'u')) 
			amount /= 2;
		}
	if POWER(m,'i') 
		amount = 0;
	if (amount<0) 
		amount = 0;
	return(amount);
}
