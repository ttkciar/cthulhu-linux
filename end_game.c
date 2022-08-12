#include "end_game.h"
#include "cthulhu.h"
#include "init.h"
#include "misc.h"
#include "graphics.h"
#include "file_func.h"

void Quit(int x) {
	ask("Are you sure ?");
	if (getchar() != 'y') 
		return;
	ask("Save game ?");
	if (getchar() == 'y') 
		save();
	player.body=0;
	sprintf(death,"quit playing");
	endgame(0);
}

void endgame(int saved) {  /* 1 if saved-game, 0 otherwise */
	char string[80];
	register int i;
	char j;
	FILE *fp;
	static struct high_score {
		char name[10];
		int score;
		char killer[40];
		char city[30];
		} t, hs[MAX_ON_HS] = {" ", 0, " ", " "};

	if (saved < 2) 
	{
		clear();
		refresh();
#ifndef NOCURSES
		endwin();
#endif
	}
#ifndef SYSV
	tsetgid(GROUP);
#endif
	if (saved == 0) 
	{
		if (player.body<1) 
		{
			printf("you died\n");
			if (strlen(death)<1) 
				sprintf(death, "died of unknown causes");
		} 
		else 
		{
			printf("you're insane\n");
			if (strlen(death)<1) sprintf(death, "went insane");
		} 
	} 
	else if (saved == 1) 
		sprintf(death, "%s%s%s", "saved game (version ", VERSION, ")");
	printf("score = %d\n", player.score);
	printf("%s %s\n",player.name,death);
#ifndef DEBUG
	if ((fp = fopen(HIGH_SCORE, "rb")) == NULL) 
	{
            if (errno == EACCES)
            {
            	printf("Does not have permission to open high score file.\n");
                exit(1);
            }
            else
            {
	    	printf("initializing high score list.\n");
                printf("Please check for chmod.\n");
                COUNT_DOWN ( i, MAX_ON_HS ) 
                {
		    hs[i].score = 0;
		    sprintf(hs[i].name, " ");
		    sprintf(hs[i].killer, " ");
		    sprintf(hs[i].city, "");
                }
            }
	} 
	else
        {
	    fread((char *) hs, sizeof(hs), 1, fp);
        }

	for (j=hs[0].score, i=1; i<MAX_ON_HS; i++)
	    if (j == 0) 
	    {
		    hs[i].score = 0;
		    sprintf(hs[i].name, " ");
		    sprintf(hs[i].killer, " ");
		    sprintf(hs[i].city, "");
	    } 
	    else 
		    j=hs[i].score;

	for (j=MAX_ON_HS, i=0; i<MAX_ON_HS; i++)
	    if (strcmp(player.name, hs[i].name)==0 || i==MAX_ON_HS-1) 
	    {
		if (player.score > hs[i].score) 
		{
			hs[i].score = player.score;
			sprintf(hs[i].name, "%s", player.name);
			sprintf(hs[i].killer, "%s", death);
			sprintf(hs[i].city, "%s", cty[player.city].name);
			j=i;
			while (j>0 && hs[j].score > hs[j-1].score) 
			{
				memcpy((char *) &t, (char *) &hs[j-1], 
					sizeof(struct high_score));
				memcpy((char *) &hs[j-1], (char *) &hs[j], 
					sizeof(struct high_score));
				memcpy((char *) &hs[j], (char *) &t, 
					sizeof(struct high_score));
				j--;
			}
		}
		i=MAX_ON_HS;
	    }

	if ((fp = fopen(HIGH_SCORE, "w+b")) == NULL)
        {
	    printf("Can't write to high score list.\n");
            printf("Please check permissions.\n");
        }
        else
        {
            fwrite((char *) hs, sizeof(hs), 1, fp);
            fclose(fp);
        }

	for (i=0; i<highlist; i++) 
	    if (hs[i].score > 0) 
		printf("%3.d) %10.d %10.10s %s in %s\n", i+1, hs[i].score,
			hs[i].name, hs[i].killer, hs[i].city);

	if (j>highlist-1 && j<MAX_ON_HS && hs[j].score > 0) 
		printf("%3.d) %10.d %10.10s %s in %s\n", j+1, hs[j].score, 
			hs[j].name, hs[j].killer, hs[j].city);
#endif
	exit(0);
}
