#include "graphics.h"
#include "cthulhu.h"
#include "init.h"
#include "misc.h"

/*
 * This routine updates the changing information on the main data screen.
 */
void upscreen(void) {
        int the_day  = player.time / 24 + 1;
        int the_hour = player.time % 24;
        char *corruption_name = dclevel(corr(-1));
        char pad[8];
        strcpy(pad, "  ");
        if (strlen(corruption_name) > 6) pad[0] = (char)0;

	move(0, 6);
	printw("%s", cty[player.city].name);
	move(1, 12);
	printw("%s%s", dclevel(corr(-1)), pad);
	move(2, 7);
	printw("%d", player.score);
	move(3, 0);
	printw("%d/%d ", player.body, player.topbody);
	move(4, 0);
	printw("%d ", player.sanity);
	move(5, 0);
	printw("%c%d ", '$', player.money);
	move(6, 0);
	printw("%d ", player.str);
	move(7, 0);
	printw("%d ", player.dex);
	move(8, 0);
	printw("%d ", player.intel);
	move(9, 0);
	printw("%3d %02d ", the_day, the_hour);
	move(10, 0);
	printw("%s ", (player.food == 0)?
	    "Healthy":(player.food>30)?"Starving":"Hungry");
}

/*
 * This segment just dumps the background/static/placement info on the screen
 */
void fillscreen(void) {
	move(0,0);
	addstr("City: \n");
	addstr("Corruption: \n");
	addstr("Score: \n");
	addstr("          : Hit Points\n");
	addstr("          : Sanity\n");
	addstr("          : Money\n");
	addstr("          : Strength\n");
	addstr("          : Dexterity\n");
	addstr("          : Knowledge\n");
	addstr("          : Time\n");
}

/*
 * Places a question on the portion of the screen reserved for questions,
 * which is the bottom left hand corner.
 */
void ask(char *str) {
	move((LINES-3),(COLS-30));
	clrtoeol();
	move((LINES-3),(COLS-30));
	addstr(str);
	refresh();
	for (st_pos=LINES; st_pos>(LINES-4); st_pos--) 
	{
	    move(st_pos,0); 
	    clrtoeol(); 
	}
	st_pos++;
}

 /* compose a list to be displayed on the screen. Giving it a null starts the
  * list, and any other string is added to the list. A refresh must be called
  * afterwards as it will not call on itself.
  */
void composelist(char *str) {
	static int composelist = 1;
        if (str == NULL)
        {
                COUNT_DOWN (composelist, LINES - 4) 
                {
                        move(composelist, 27);
                        clrtoeol();
                }
        	composelist = 1;
                return;
        }       
	move(composelist % (LINES - 4), 27 + 20 * (composelist / (LINES - 4)));
        clrtoeol();
        addstr(str);
        composelist++;
}

/*
 * This puts the help screens (both of them) up on the screen.
 */
void help(void) {
	clear();
	move(0,0);
	addstr("A check into an Asylum for $1000 (can be risky).\n");
	addstr("b go to bed (until something wakes you).\n");
	addstr("B go to bed for one night.\n");
	addstr("d drink from a cup.\n");
	addstr("f find those blasphemous foes and fight them.\n");
	addstr("h heal up your wounds.\n");
	addstr("H check into a Hospital for $1000 (can be risky).\n");
	addstr("i inventory your belongings.\n");
	addstr("l look up your posessions in a library.\n");
	addstr("L camp out in the library until you know all about your posessions.\n");
	addstr("q fight against evil hopeless, commit suicide.\n");
	addstr("r rest/sleep up and restore yourself.\n");
	ask("Press any key for next screen.\n");
	WAIT();
	clear();
	addstr("R redraw screen\n");
	addstr("s search for something to use in your battle against evil.\n");
	addstr("S cast spells. ('c' in combat)\n");
	addstr("t travel to another city by walking(hitch-hiking), \n");
	addstr("	bus(boat), train(steamer), or plane\n");
	addstr("u use an item\n");
	addstr("w wield a weapon\n");
	addstr("? print out this list.\n");
	addstr("$ try to earn some quick cash doing manual-labor.\n");
	addstr("^ spend a day in the library doing research.\n");
	addstr("# work until bothered by a monster.\n");
	ask("Press any key to get back to game.\n");
	WAIT();
	clear();
}
