#include "travel.h"
#include "cthulhu.h"
#include "init.h"
#include "misc.h"
#include "graphics.h"
#include "objects.h"
#include "clock.h"

int list(int t, int flag) {
	register int i, j;
	int d, d1, d2, tmcst, mncst;
	int scrflg;

	scrflg=0;
	j = 0;
	COUNT_DOWN ( i, NCITIES ) 
	{
		if (cty[player.city].lon > 360 || cty[i].lon > 360) 
		{
			d= -1;
			tmcst = 0;
			mncst = 0;
		} 
		else 
		{
			d1=cty[player.city].lon-cty[i].lon;
			d2=cty[player.city].lat-cty[i].lat;
			d=(cty[player.city].lat+cty[i].lat);
			d1=ABS(d1);
			d2=ABS(d2);
			d=ABS(d);
			if (d1>180) 
				d1=360-d1;
			d1 = (d1 * 120) / (d + 120);
			d=nsqrt(d1*d1 + d2*d2);
			tmcst=(48 * d) / t;
			mncst=((3*d + t) * (t-10)) / 20;
		}
		if (player.city != i 
		    && d < (t+1) 
		    && ((flag == 2 && cty[i].lon>360) || (d > 0 && flag<2)) 
		    && (flag > 0 || mncst < player.money + 1)) 
		{
			if (scrflg==0) 
			{
                        	composelist(NULL);
				scrflg=1; 
			}
			tpc[j]=tmcst;
			cpc[j]=mncst;
			cp[j]=i;
			j++;
			if (flag == 0 && mncst == 0) 
				sprintf(tmp, "%c) %d hours %s", ITOLISTICON(j),
					tmcst, cty[i].name);
			else if (flag == 0) 
				sprintf(tmp, "%c) %d hours $%d %s", 
                                	ITOLISTICON(j), tmcst, mncst, 
                                        cty[i].name);
			else 
				sprintf(tmp, "%c) %s", ITOLISTICON(j), 
					cty[i].name);
                        composelist(tmp);        
		}
	}
	if (j>0) 
	{
		ask("What city : ");
		i=getitem();
		clear();
	}
	pc = j;
	return(i);
}

void travel(void) {
	char c, type;

	ask("w)alk b)us t)rain p)lane: ");
	c=getchar();
	switch (c) 
	{
		case 'w': c=list(10, 0); type = 1; break;
		case 'b': c=list(20, 0); type = 2; break;
		case 't': c=list(40, 0); type = 3; break;
		case 'p': c=list(80, 0); type = 4; break;
		default: return;
	}
	if (pc>0) 
	{
		if (c < 1 || c>pc) 
			return;
		else 
			c--;
		if (cpc[c]>player.money) 
		{
			STATE("Not enough money");
			return;
		} 
		else 
		{
			player.money -= cpc[c];
			traveltime(tpc[c], type);
			sprintf(tmp, "The journey takes %d hours.",tpc[c]);
			STATE(tmp);
			player.city = cp[c];
		}
	} 
	else 
		STATE("No cities within range.");
	return;
}
