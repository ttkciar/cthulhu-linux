#include "cthulhu.h"
#include "main.h"
#include "init.h"
#include "misc.h"
#include "graphics.h"
#include "clock.h"
#include "heal.h"
#include "objects.h"
#include "spell.h"
#include "travel.h"
#include "monsters.h"
#include "end_game.h"
#include "start_game.h"
#include "file_func.h"

/*
 * Figure out what the user should be called.
 */
void get_username(void) {
	char *getlogin();
#ifdef SYSV
//      /* Try to get it for cuserid from utmp */
//      cuserid(player.name);
//
//	/* If that fails, grab it from the password file */
//	if (player.name[0] == (char) 0)
//	{
		struct passwd *pswd;
		pswd = getpwuid(getuid());
		strcpy(player.name, pswd->pw_name);
//	}
#else
      	char *new_name = getlogin();

	/* If that fails, grab it from the password file */
	if (new_name == NULL)
	{
		struct passwd *pswd;
		pswd = getpwuid(getuid());
		strcpy(player.name, pswd->pw_name);
	} 
        else
        {
        	strcpy(player.name, new_name);
        }
#endif


	/* Barring that, assign an annonymous name */
	if ( player.name[0] == (char) 0 )
	{
		strcpy(player.name, "John Doe");
	}
}

/*
 * This is the main loop of the game. Each iteration of the loop represents
 * an hour (unless the user miss-typed and the command was garbage). The
 * key-entries are parsed and operated on here via sub-routines. At the end
 * there are jumps to cron-like subroutines that operate every x calls.
 */
void playgame(void) {
	char c;
	static char bed = 0, library = 0, manlab = 0;

	if (bed == 0 && library == 0 && manlab == 0) 
	{
	    upscreen();
	    ask("Decision: ");
	    c=getchar();
	    switch (c) 
	    {
		case 'A': 
			asylum(); 
			CthulhuClock(); 
			break;
		case 'H': 
			hospital(); 
			CthulhuClock(); 
			break;
		case 'b': 
			STATE("You go to sleep.\n"); 
			bed = 1; 
			break;
		case 'B': 
			STATE("You go to bed for the night.\n"); 
			bed=25; 
			break;
		case 'd': 
			drink(); 
			CthulhuClock(); 
			break;
		case 't': 
			travel(); 
			fillscreen(); 
			break;
		case 'w': 
			wield(); 
			break;
		case 's': 
			search(0, 0); 
			CthulhuClock(); 
			break;
		case 'i': 
			displayobj(0); 
			break;
		case 'c': // TTK here, making consistent with combat spell-casting
		case 'S': 
			castspell(); 
			break;
		case '?': 
			help(); 
			fillscreen(); 
			break;
		case 'u': 
			useobj(); 
			CthulhuClock(); 
			break;
		case 'r': 
			rest(5); 
			CthulhuClock(); 
			break;
		case 'R': 
			clear(); 
			fillscreen(); 
			break;
		case 'l': 
			lookup(0); 
			CthulhuClock(); 
			break;
		case 'L':
                	composelist(NULL);
			library = 1; 
			break;
		case 'f': 
			monster(player.intel); 
			CthulhuClock(); 
			break;
		case 'h': 
			heal(5); 
			CthulhuClock(); 
			break;
		case '$': 
			EARN_MONEY(); 
			CthulhuClock(); 
			break;
		case '#': 
			manlab = 1; 
			break;
		case '^': 
			library = 25; 
			break;
		case 'q': 
			Quit(0); 
			break;
		case ' ': 
			break;
		default : 
			STATE("I don't know that one."); 
			break;
	}
    }
    else if (player.sanity > 0 && player.body > 0) 
    {
	/* If I'm sleeping or studing or working */
	if (bed > 0) 
	{
	    rest(5);
	    if (player.body > ((player.topbody * 9) /10) &&
		player.sanity > ((player.topsan * 9) /10)) 
		{
		    STATE("You woke up.\n");
		    bed = 0;
		}
	    if (bed > 1) 
	    {
		if (--bed == 1) 
		{
		    STATE("You woke up.\n");
		    bed = 0;
		}
	    }
	} 
	else if (library > 0) 
	{
	    if (lookup(1)==2) 
            	    library=0;
	    if (library > 1) 
	    {
		    if (--library == 1) 
		    {
			    STATE("You leave the library.\n");
			    library = 0;
		    }
	    }
	} 
	else if (manlab > 0) 
	    EARN_MONEY();

	if (CthulhuClock() > 0) 
	{
	    bed = 0;
	    library = 0;
	    manlab = 0;
	}
    }
}

/*
 * This is called if the program can't cope any more and needs to quit.
 */
void bailout(void) {
	addstr("Can't continue with game\n\n\n");
	endgame(0);
}

/*
 * This sets up the sign_index, which points to all the Elder signs in the
 * game. This is used later on.
 */
void assign(void) {
	int i;
        int j = 0;

	COUNT_DOWN(i, NOBJECTS)
	    if (GIFT(i, 'E'))
		sign_index[j++] = i;
}

int main(int argc, char **argv) {
	int c;
	enum {new_game, restored_game, anonymous_game, game_over} game_status;
	extern char *optarg;
	extern int optind;

	srand(getpid() + time(NULL));
	get_username();
	game_status = new_game;
	while ((c = getopt(argc, argv, "hnurS:I:D:")) != -1)
	    switch (c)
	    {
		case 'h':
		    highlist=MAX_ON_HS;
		    player.score = 1;
		    sprintf(death, "%s", "checking high score list");
		    game_status = game_over;
		    break;
		case 'u':
		    player.score = 1;
		    sprintf(death, "%s", 
			"tried to understand what this thing is");
		    printf("%s: %s\n", argv[0], "[-(hurn)] [-[SID]n]");
		    printf("\t-h   print all high scores\n");
		    printf("\t-u   print this message\n");
		    printf("\t-r   restart old game\n");
		    printf("\t-n   use anonymous name\n");
		    printf("\t-S n set starting strength to n\n");
		    printf("\t-I n set starting intelligence to n\n");
		    printf("\t-D n set starting dexterity to n\n");
		    game_status = game_over;
		    break;
		case 'r':
		    game_status = restored_game;
		    break;
		case 'n':
		    {
		    static char *tmpname[] = {	    /* no more than 10 char */
			    "Doc Savage",
			    "Brunhilda",
			    "B. Bonzai",
			    "Batman",
			    "L. Niven",
			    "Prof. F",
			    "Zonker",
			    "Lovecraft",
			    "Zamphir",
			    "Whorfin",
			    "Bigboote",
			    "Morticia",
			    "Merlin",
			    "Tom Waits",
			    "Elvira"};
		    strcpy(player.name, tmpname[rand() % 15]);
		    game_status = anonymous_game;
		    break;
		    }
		case 'S':
		    {
			int str;
			str = atoi(optarg);
			if (str >= 6 && str < 15) player.str = str;
			break;
		    }
		case 'I':
		    {
			int intel;
			intel = atoi(optarg);
			if (intel >= 6 && intel < 15) player.intel = intel;
			break;
		    }
		case 'D':
		    {
			int dex;
			dex = atoi(optarg);
			if (dex >= 6 && dex < 15) player.dex = dex;
			break;
		    }
	    } 	/* while reading getopts */

	while ((c = player.str + player.intel + player.dex) != 30)
	    switch (rand() % 3)
	    {
		case 0: 
		    player.dex = player.dex + ((c>30) ? (-1):(1));
		    break;
		case 1: 
		    player.str = player.str + ((c>30) ? (-1):(1));
		    break;
		case 2: 
		    player.intel = player.intel + ((c>30) ? (-1):(1));
		    break;
	    }

	switch (game_status)
	{
	    case game_over:
		endgame(2);
		break;
	    case new_game:
	    case anonymous_game:
		begin();
		startgame();
		break;
	    case restored_game:
		begin();
		retrieve();
		fillscreen();
		break;
	    default:
		sprintf(death, "Unknown game mode.");
		endgame(2);
		break;
	}

	assign();

	while (player.sanity > 0 && player.body > 0) 
	    playgame();
	endgame(0);
}
