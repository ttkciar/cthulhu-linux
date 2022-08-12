#include "file_func.h"
#include "cthulhu.h"
#include "init.h"
#include "misc.h"
#include "end_game.h"
#include "main.h"
#include "graphics.h"

void save(void) {
	FILE *save, *fopen();
	char path[MAXPATH];
	char *homedir, *getenv();
        char version_buffer[VERSION_MAX_LEN+1];

	if ((homedir = getenv("HOME")) == (char *) NULL) 
	{
	    addstr("No HOME environment variable set - Cannot save game\n");
	    Quit(0);
	} 
	else 
	{
	    sprintf(path, "%s%s", homedir, SAVEGAME);
	    move(21, 0); clrtoeol();
	    printw("Saving Game to %s\n", path);
	    if ((save = fopen(path, "wb")) == NULL) 
	    {
                if (errno == EACCES)
                {
			sprintf(death, "incorrect permissions to save game");
                }
		else
                {
			sprintf(death, "pissed at savegame(%d) function", 
                        	errno);
                }
		endgame(0);
	    }
	}
	fwrite((char *) &player, sizeof(player), 1, save);
	fwrite((char *) cty, sizeof(cty), 1, save);
	fwrite((char *) obj, sizeof(obj), 1, save);
	fwrite((char *) mon, sizeof(mon), 1, save);
	fwrite((char *) spell, sizeof(spell), 1, save);
	fwrite((char *) &goal, sizeof(unsigned int), 1, save);

        strcpy(version_buffer, (char*)VERSION);
	fwrite(version_buffer, sizeof(char) * VERSION_MAX_LEN, 1, save);

	fclose(save);
	if (tell_guards(getuid(),1) == 0) /* write uid in savegame check file */
	{	
	    while(player.sanity > 0 && player.body > 0) 
		playgame();
	    endgame(0);
	} 
	else 
	    endgame(1);
}

void retrieve(void) {
	FILE *save, *fopen();
	char path[MAXPATH];
        char tmp_version[VERSION_MAX_LEN+1];
	char *homedir, *getenv();

	if ((homedir = getenv("HOME")) == (char *) NULL) 
	{
	    addstr("No HOME environment variable set - Cannot save game\n");
            WAIT();
	    Quit(0);
	} 
	sprintf(path, "%s%s", homedir, SAVEGAME);
	move(21, 0); clrtoeol();
	printw("Loading Game from %s\n", path);
	if (tell_guards(getuid(),2) == 0) 
	{
		addstr("error - You're not an undead, lie still.");
		WAIT();
		sprintf(death, "caught lying to computer");
		bailout();
	} 
	else if ((save = fopen(path, "rb")) == NULL) 
	{
        	if (errno == EACCES)
                {
                    sprintf(death, "incorrect permissions to restore game");
                }
                else if (errno == ENOENT)
                {
                    sprintf(death, "no graveyard to steal bodies from");    
                }
                else
                {
                    sprintf(death, "graverobbers took %d corpses", errno);
                }
                addstr(death);
                addstr("\n");
                WAIT();
		bailout();
	}
	fread((char *) &player, sizeof(player), 1, save);
	fread((char *) cty, sizeof(cty), 1, save);
	fread((char *) obj, sizeof(obj), 1, save);
	fread((char *) mon, sizeof(mon), 1, save);
	fread((char *) spell, sizeof(spell), 1, save);
	fread((char *) &goal, sizeof(unsigned int), 1, save);
	fread((char *) tmp_version, sizeof(char) * VERSION_MAX_LEN, 1, save);
	fclose(save);
	if (strcmp(tmp_version, VERSION) != 0) 
	{
	    addstr("Whoops! This save game is from a different version.\n");
            WAIT();
	    bailout();
	} 
	else 
	    unlink(path);
}

int tell_guards(unsigned short uid, int checkbit) {
        // checkbit == 1 if writing, 2 if reading
	static unsigned short last = 0, undead[MAX_SAVERS] = {0};
	int i = 0, poss = -1;
	FILE *fp;

//#ifndef SYSV
//	setgid(GROUP)
//#endif
        WAIT();
	if ((fp = fopen(SAVE_LIST, "rb")) == NULL) 
	{
        	if (errno == EACCES)
                {
                    addstr("error - save game guards don't like you.\n");
                    WAIT();
                    return 0;
                }
                else if (errno == ENOENT)
                {
                    addstr("initializing save game list.\n");
                    addstr("Check ");
                    addstr(SAVE_LIST);
                    addstr(" permissions.\n");
                    WAIT();
                }
                else
                {
                    addstr("error - guards found rotting at their posts.\n");
                    WAIT();
                    return 0;
                }
	} 
	else 
	{
		fread((char *) undead, sizeof(undead), 1, fp);
	}

	while (i<MAX_SAVERS && undead[i] != 0) 
	{
		last = undead[i];
		if (undead[i] == uid) poss = i;
		i++;
	}

	switch (checkbit) 
	{
	    case 1:	
		if (i == MAX_SAVERS) 
                {
                	addstr("error - too many undead already, no cold graves.");
                        WAIT();
			return 0; 
                }
		else if (poss == -1) 
                {
                        
			undead[i]=uid;
                }
		else 
		{
			addstr("obliterating slow zombie\n");
			WAIT();
		}
		break;
	    case 2: 
		if (poss == -1) 
			return 0;
		undead[poss]=last;
		undead[i-1]=0;
		break;
	    default: 
		addstr("error: strange signal given to guards\n");
		WAIT();
		break;
	}

	if ((fp = fopen(SAVE_LIST, "w+b")) == NULL) 
	{
        	if (errno == EACCES)
                {
                	addstr("error - elder sign found on undead grave.\n");
                }
                else
                {
			addstr("error - can't dig draves for undead.\n");
                }
		WAIT();
                return 0;
	}
	fwrite((char *) undead, sizeof(undead), 1, fp);
	fclose(fp);
	return 1;
}
