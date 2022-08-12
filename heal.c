#include "heal.h"
#include "cthulhu.h"
#include "init.h"
#include "misc.h"
#include "graphics.h"
#include "clock.h"

/*
 * If the player chooses to heal, the damage is figured in this
 * routine.
 */
void heal(int restful) {
	int chance;

	restful = 400 / (restful * (1 + player.ability[AREGEN])); 
	chance = (100 * (player.topbody - player.body)) / player.topbody;
	if (rand() % restful < chance) 
		player.body++;
}

/*
 * If the player chooses to rest, the damage is figured in this
 * routine.
 */
void rest(int restful) {
	int chance;
	
	if (player.food == 0) restful++;
	restful = 800 / (restful * (1 + player.ability[AREGEN])); 
	chance = (100 * (player.topbody - player.body)) / player.topbody;
	if (rand() % restful < chance && player.body < player.topbody) 
		player.body++;
	chance = (100 * (player.topsan - player.sanity)) / player.topsan;
	if (rand() % restful < chance && player.sanity < player.topsan) 
		player.sanity++;
}

/*
 * If the player chooses to go to an asylum, the damage is figured in this
 * routine.
 */
void asylum(void) {
	char am;
	
	if (player.money < 1000) 
	{
		STATE("You don't have enough money to check into an asylum.");
		return;
	}
	player.money -= 1000;
	burntime(200);
	am = ((rand() % 10) - 1) * (10 + MOD_16(rand()));
	if (am < 0) 
	{
		sprintf(death,"locked up in an asylum");
		STATE("Well, that certainly didn't help.");
	} 
	else if (am < 20) 
	{
		STATE("That did very little for your mental health.");
		am = 20;
	}
	else 
	{
		STATE("You come out a changed person.");
	}
	player.sanity += am;
	if (player.topsan < player.sanity) 
		player.sanity = player.topsan;
	else 
		player.topsan -= 10;
}

/*
 * If the player chooses to go to a hospital, the damage is figured in this
 * routine.
 */
void hospital(void) {
	char am;
	void display_plagues();
	
	if (player.money < 1000) 
	{
		STATE("You don't have enough money to go to a hospital.");
		return;
	}
	player.money -= 1000;
	burntime(200);
	am = ((rand() % 10) - 1) * (10 + MOD_16(rand()));
	if (am < 0) 
	{
		sprintf(death, "died under the knife at the hospital");
		STATE("Well, that certainly didn't help.");
	} 
	else if (am < 20) 
	{
		STATE("That did very little for your health.");
		am = 20;
	}
	else 
	{
		STATE("You feel great.");
	}
	player.body += am;
	if (player.topbody < player.body) 
		player.body = player.topbody;
	else 
		player.topbody -= 10;
	display_plagues("The doctors detect the following on you:");
}

void display_plagues(char *plague_string) {
	int i, has_plague = 0;
	composelist(NULL);
	composelist(plague_string);
	COUNT_DOWN (i, NPLAGUE)
                if (plague[i].caught == 1)
                {
                	has_plague = 1;
                	composelist(plague[i].name);
                }
        if (has_plague == 0)
        	composelist(NULL);
}
