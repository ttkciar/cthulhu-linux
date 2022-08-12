#include "cthulhu.h"
#include "init.h"

struct plague_table plague[NPLAGUE] = {
#ifdef DEBUG
    1,"Plague",
    0,"Leprosy",
    0,"Cancer",
    0,"Rotting Flesh",
    0,"an Inhuman condition",
    1,"Black Death",
    0,"the Walking Death",
    1,"a Brain Tumor",
    0,"Brain larva",
    0,"Brain Rot"};
#else
    0,"Plague",
    0,"Leprosy",
    0,"Cancer",
    0,"Rotting Flesh",
    0,"an Inhuman condition",
    0,"Black Death",
    0,"the Walking Death",
    0,"a Brain Tumor",
    0,"Brain larva",
    0,"Brain Rot"};
#endif

char *dlev[NCORRUPTION] = {
	"Normal", "Odd", "Strange", "Weird", "Inhuman",
	"Very Disturbing", "Totally Alien", "Blasphemous", "Hellish"};

char *headmaster[NDIMS] = {
	"Cthulhoo", "Nyerlethotap", "Ithakwa", "Daagon", 
	"Hasteur", "a Star Vampyr", "Tshathogha", "Formless Spawn", 
	"Nyagoetha", "Azartathoth", "Yug-Zothugh", "a Flying Tendril"};

#ifdef DEBUG
unsigned int goal = 0x6ff;	/* bitmap of current headmasters' defeated */
#else
unsigned int goal = 0x0;	/* bitmap of current headmasters' defeated */
#endif

unsigned int end_goal = 0xfff;	/* All 12 headmasters bits set */ 

struct fight_status ft_stat = {0};

struct player_table player = {
#ifdef DEBUG
    1000, 2000, 1000, 2000, 5000, 0, 2000,
    1, 50, 50, 50, 0, 0, 10000,
    HANDS, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    "Cthulhoo"};
#else
    100, 150, 100, 150, 200, 0, 0,
    1, 10, 10, 10, 0, 0, 0,
    HANDS, 1, 1, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0,
    "Cthulhoo"};
#endif

struct city_table cty[NCITIES] = {
    "London", 51, 0, 0, 0, 0,
    "Stonehenge", 53, 2, 0, 0, 2,
    "Casablanca", 33, 6, 0, 0, 0,
    "Reykjavik", 70, 20, 0, 0, 0,
    "the Amazon", -4, 55, 0, 0, 3,
    "Tierra Del Fuego", -55, 68, 0, 0, 1,
    "Antarctica", -90, 70, 0, 0, 3,
    "Innsmouth", 41, 70, 0, 0, 1,
    "Dunwitch", 43, 70, 0, 0, 3,
    "Boston", 42, 71, 0, 0, 1,
    "New York", 40, 73, 0, 0, 0,
    "Arkham", 42, 73, 0, 0, 1,
    "New Orleans", 29, 90, 0, 0, 0,
    "Devil's Tower", 41, 109, 0, 0, 2,
    "Easter Island", -24, 111, 0, 0, 2,
    "San Francisco", 37, 112, 0, 0, 0,
    "Death Valley", 39, 116, 0, 0, 1,
    "Los Angeles", 34, 118, 0, 0, 0,
    "Salem", 44, 123, 0, 0, 2,
    "Anchorage", 61, 149, 0, 0, 1,
    "Honolulu", 21, 157, 0, 0, 0,
    "Sydney", -33, 209, 0, 0, 0,
    "Tokyo", 35, 221, 0, 0, 0,
    "Hong Kong", 22, 247, 0, 0, 1,
    "Singapore", 1, 256, 0, 0, 0,
    "Bangkok", 13, 259, 0, 0, 1,
    "Bombay", 18, 288, 0, 0, 0,
    "Mecca", 20, 319, 0, 0, 1,
    "Moscow", 55, 323, 0, 0, 0,
    "Murmansk", 68, 327, 0, 0, 0,
    "Cairo", 30, 328, 0, 0, 1,
    "Athens", 37, 337, 0, 0, 0,
    "Berlin", 52, 347, 0, 0, 0,
    "Rome", 41, 348, 0, 0, 0,
    "Paris", 48, 358, 0, 0, 0,
    "R'yleh", -1, 361, 0, 0, 7,
    "Irem, city of pillars", -2, 362, 0, 0, 4,
    "The Cloud of Oort", -3, 363, 0, 0, 5,
    "The Mountains of Madness", -4, 364, 0, 0, 5,
    "Limbo", -5, 365, 0, 0, 4,
    "Pluto", -6, 366, 0, 0, 5,
    "Dreamlands", -7, 367, 0, 0, 6,
    "N'kai", -8, 368, 0, 0, 6,
    "Plateau of Leng", -9, 369, 0, 0, 4,
    "Center of the Universe", -10, 370, 0, 0, 9,
    "Gateway to the Dimensions", -11, 371, 0, 0, 8,
    "Underground", -12, 372, 0, 0, 4};

struct object_table obj[NOBJECTS] = {
/* s Gain strength		i Gain intelligence */
/* d Gain dexterity		r Heal body */
/* S heal sanity		t teleportation */
/* a armor			m keep sanity */
/* h holyness (anti-undead)	k gain spells */
/* H learning 			g summoning */
/* _ nothing			v vampirism */
/* P purification		C curse town (raise corruption level) */
/* D dimension shifting		$ money */
/* 0-9 safety from dimension x 	T see invisible */
/* w resist hypnosis and howls  G get random object */
/* b banish monster 		B raise abilities */
/* E Elder Sign			n can't add str to weapon damage */
/* c cure disease 		u unholy weapon */
    "strength tattoo", "a tattoo", 's', 1, 1, 0, 100, 1, 0, 0,
    "intelligence tattoo", "a tattoo", 'i', 1, 1, 0, 100, 1, 5, 0,
    "dexterity tattoo", "a tattoo", 'd', 1, 1, 0, 100, 1, 0, 0,
    "healing tattoo", "a tattoo", 'r', 1, 1, 0, 100, 1, 5, 0,
    "sanity tattoo", "a tattoo", 'S', 1, 1, 0, 100, 1, 0, 0,
    "teleportation tattoo", "a tattoo", 't', 1, 1, 0, 100, 1, 10, 0,
    "armor tattoo", "a tattoo", 'a', 1, 1, 0, 0, 1, 0, 0,
    "mental health tattoo", "a tattoo", 'm', 1, 1, 0, 0, 1, 10, 0,
    "holyness tattoo", "a tattoo", 'h', 1, 1, 0, 0, 1, 20, 0,
    "knowledge tattoo", "a tattoo", 'k', 1, 1, 0, 200, 1, 5, 0,
    "learning tattoo", "a tattoo", 'H', 1, 1, 0, 0, 1, 15, 0,
    "summoning tattoo", "a tattoo", 'g', 1, 1, 0, 150, 1, 10, 0,
    "total sight tattoo", "a tattoo", 'T', 1, 1, 0, 0, 1, 20, 0,
    "iron will tattoo", "a tattoo", 'w', 1, 1, 0, 0, 1, 5, 0,
    "purification tattoo", "a tattoo", 'P', 1, 1, 0, 200, 1, 5, 0,
    "a normal sword", "a sword", ' ', 2, 5, 0, 0, 1, 0, 0,
    "a magic sword", "a sword", ' ', 3, 5, 0, 0, 1, 0, 0,
    "a silver sword", "a sword", 'h', 2, 6, 0, 0, 1, 0, 0,
    "a sword of destruction", "a sword", ' ', 3, 8, 0, 0, 1, 0, 0,
    "a normal dagger", "a dagger", ' ', 2, 2, 0, 0, 1, 0, 0,
    "a dagger of vampirism", "a dagger", 'v', 2, 4, 0, 0, 1, 5, 0,
    "a sacrificial blade", "a dagger", 'h', 4, 2, 0, 0, 1, 0, 0,
    "an ordinary axe", AXE, ' ', 2, 3, 0, 0, 1, 0, 0,
    "a book of spells", "a book", 'k', 1, 1, 0, 1, 0, 5, 0,
    "a book of cursing", "a book", 'C', 1, 1, 0, 1, 0, 10, 0,
    "a book of summoning", "a book", 'g', 1, 1, 0, 1, 0, 5, 0,
    "a book of secrets", "a book", 'k', 1, 1, 0, 1, 0, 15, 0,
    "a book of dimension shifting", "a book", 'D', 1, 1, 0, 4, 0, 20, 0,
    "the Necronomicon", "a book", 'k', 1, 1, 0, 5, 0, 10, 0,
    "Ichorous Grail", "a cup", '1', 1, 1, 0, 0, 1, 15, 0,
    "Glass of Wine", "a cup", '2', 1, 1, 0, 0, 1, 15, 0,
    "Vial of Blood", "a cup", '3', 1, 1, 0, 0, 1, 15, 0,
    "Empty Glass", "a cup", '4', 1, 1, 0, 0, 1, 15, 0,
    "Golden Goblet", "a cup", '5', 1, 1, 0, 0, 1, 15, 0,
    "Cup of Fire", "a cup", '6', 1, 1, 0, 0, 1, 15, 0,
    "Crystal Vase", "a cup", '7', 1, 1, 0, 0, 1, 15, 0,
    "Glass of Vengeance", "a cup", '8', 1, 1, 0, 0, 1, 15, 0,
    "Twisted Chalice", "a cup", '9', 1, 1, 0, 0, 1, 15, 0,
    "Wooden Flask", "a cup", '0', 1, 1, 0, 0, 1, 15, 0,
    "knowledge pipe", PIPE, 'k', 1, 1, 0, 1, 0, 10, 0,
    "summoning pipe", PIPE, 'g', 1, 1, 0, 1, 0, 10, 0,
    "dreaming pipe", PIPE, 'G', 1, 1, 0, 1, 0, 10, 0,
    "banishing pipe", PIPE, 'b', 1, 1, 0, 1, 0, 10, 0,
    "benefit pipe", PIPE, 'B', 1, 1, 0, 1, 0, 10, 0,
    "cure disease pipe", PIPE, 'c', 1, 1, 0, 1, 0, 10, 0,
    "a Shotgun", "a gun", 'n', 2, 8, 0, 0, 1, 0, 0,
    "a Pistol", "a gun", 'n', 1, 8, 0, 0, 1, 0, 0,
    "a Rifle", "a gun", 'n', 1, 12, 0, 0, 1, 0, 0,
    "a Blow Gun", "a gun", 'n', 1, 6, 0, 0, 1, 0, 0,
    "a sword of heresy", "a sword", 'u', 4, 6, 0, 0, 1, 15, 0,
    "a blackened dagger", "a dagger", 'u', 3, 3, 0, 0, 1, 15, 0,
    "dragon sign", SIGN, 'E', 1, 1, 0, -1, 200, 20, 0,
    "fire sign", SIGN, 'E', 1, 1, 0, -1, 200, 30, 0,
    "star sign", SIGN, 'E', 1, 1, 0, -1, 250, 30, 0,
    "aura sign", SIGN, 'E', 1, 1, 0, -1, 300, 30, 0,
    "mind sign", SIGN, 'E', 1, 1, 0, -1, 300, 35, 0,
    "dawn sign", SIGN, 'E', 1, 1, 0, -1, 350, 35, 0,
    "god sign", SIGN, 'E', 1, 1, 0, -1, 500, 40, 0,
    "money", "money", '$', 0, 0, 0, 1, 0, 0, 0};

struct monster_table mon[NMONSTERS] = {
/* A death Aura				W webs			*/
/* c cast wither spells            	r magic resistance 	*/
/* D damages those who touch it	   	I Invisible 		*/
/* u undead, 1/2 from non-magic	   	i only affected by spells */
/* w weapon, get weapon after fight	v vampire, gains hp from foe */
/* P travel in Packs               	V Venom 		*/
/* C carry away victim             	@ get a pipe 		*/
/* a armor                         	s summon allies 	*/
/* $ money                         	p poison 		*/
/* R regenerates                   	S susceptible to light 	*/
/* h hypnosis                      	H howls 		*/
/* G Headmaster                    	E affects protected people */
/* d disease 				t thief, steals stuff	*/
    "a hound of Tindalos", 8, 1, 12, 40, 20, 0, "Pp",
    "Nyerlethotap", 25, 25, 3, 110, 16, 0, "GcVs",
    "Hasteur", 70, 4, 18, 350, 11, 0, "ucrGAV",
    "a mi-go", 10, 1, 6, 50, 14, 0, "P",
    "Nyagoetha", 30, 10, 10, 110, 14, 0, "RGrps",
    "Tshathogha", 40, 4, 15, 200, 12, 0, "GaRpV",
    "Tiamat", 10, 2, 20, 130, 8, 0, "apc",
    "Daagon", 70, 5, 20, 200, 8, 0, "saGr",
    "Heorot", 20, 3, 10, 175, 13, 0, "ad",
    "a grendel", 10, 3, 8, 140, 13, 0, "a",
    "a madman", 2, 1, 5, 30, 10, 0, "w",
    "a lich", 20, 5, 2, 30, 18, 0, "uc",
    "a ghoul", 5, 2, 3, 50, 9, 0, "aPS",
    "a ghast", 10, 2, 2, 50, 9, 0, "apS",
    "a mad dog", 0, 1, 6, 15, 13, 0, "P",
    "a skeleton", 9, 2, 2, 20, 15, 0, "u",
    "a zombie", 5, 1, 3, 20, 4, 0, "u",
    "Cthulhoo", 75, 10, 10, 500, 8, 0, "GRscra",
    "Ithakwa", 50, 5, 12, 200, 11, 0, "uGvrRpH",
    "Windigo", 20, 2, 10, 125, 14, 0, "uRp",
    "a sasquatch", 10, 1, 10, 75, 16, 0, "u",
    "a yeti", 8, 1, 8, 80, 15, 0, "P",
    "a Dhole", 35, 5, 6, 500, 2, 0, "r",
    "a devotee", 0, 1, 4, 25, 9, 0, "$",
    "a priest", 0, 2, 3, 35, 11, 0, "$",
    "a high priest", 0, 2, 4, 50, 12, 0, "$",
    "an inhuman priest", 6, 3, 3, 68, 16, 0, "$rs",
    "a Deep One", 5, 1, 6, 45, 6, 0, "a",
    "a vampire", 8, 5, 1, 75, 21, 0, "vuS",
    "Azartathoth", 120, 3, 50, 1000, 10, 0, "GraRE",
    "a byakhee", 20, 1, 10, 125, 14, 0, "P",
    "a baby cthonian", 20, 3, 6, 20, 5, 0, "Sv",
    "a cthonian", 25, 3, 8, 70, 5, 0, "vS",
    "a large cthonian", 30, 3, 12, 220, 5, 0, "Shv",
    "spawn of cthulhu", 40, 3, 12, 200, 10, 0, "Rcra",
    "a Moon-Beast", 20, 5, 2, 80, 11, 0, "vPut",
    "Elder Thing", 20, 2, 10, 200, 10, 0, "aP",
    "a servitor", 25, 5, 3, 150, 9, 0, "@rat",
    "a Star Vampyr", 35, 10, 3, 150, 11, 0, "EGvaI",
    "a Flying Tendril", 35, 3, 16, 130, 16, 0, "GPIH",
    "a Dark Young", 20, 1, 40, 175, 9, 0, "a",
    "a dimensional shambler", 20, 2, 10, 140, 7, 0, "Ca",
    "Yug-Zothugh", 30, 20, 1, 300, 3, 0, "dDvisG",
    "Shoggoth", 30, 3, 10, 200, 10, 0, "H",
    "a hunting horror", 35, 1, 14, 100, 28, 0, "S",
    "Formless Spawn", 50, 5, 10, 200, 10, 0, "dGPiu",
    "a nightgaunt", 10, 1, 2, 100, 15, 0, "Cua",
    "an Old One", 15, 2, 10, 100, 10, 0, "as",
    "a shantak", 10, 1, 40, 200, 4, 0, "a",
    "Shub-Niggurath", 100, 10, 5, 300, 3, 0, "saR",
    "a were-wolf", 5, 2, 5, 10, 23, 0, "u",
    "a shapeless mass", 8, 2, 5, 20, 9, 0, "D",
    "an undead", 5, 1, 4, 25, 4, 0, "u",
    "a Daagonian", 5, 2, 4, 50, 6, 0, "a",
    "a creature of the UV", 8, 1, 4, 18, 16, 0, "I",
    "a Plague Cloud", 20, 1, 10, 100, 20, 0, "Ed",
    "a nosferatu", 10, 4, 2, 75, 21, 0, "vuS",
    "a rat", 1, 1, 1, 5, 10, 0, "P",
    "a Spirit", 15, 3, 5, 50, 15, 0, "iE",
    "a Stellar Guardian", 20, 5, 2, 800, 11, 0, "EAv",
    "a fanatic", 0, 1, 4, 18, 9, 0, "$",
    "a phantasm", 25, 3, 8, 50, 9, 0, "i",
    "a will-o-wisp", 18, 4, 2, 30, 13, 0, "tI",
    "a banshee", 30, 10, 5, 250, 8, 0, "Hv",
    "a metohkangi", 12, 2, 6, 80, 17, 0, "u",
    "an ichor wisp", 12, 1, 4, 30, 6, 0, "SDiv",
    "an aether wolf", 8, 3, 4, 45, 13, 0, "PH",
    "a blood elemental", 20, 3, 2, 95, 14, 0, "dR",
    "a bone demon", 15, 2, 4, 40, 15, 0, "Ha",
    "an ichor demon", 15, 3, 5, 100, 18, 0, "cSI",
    "a spirit devil", 20, 2, 10, 100, 20, 0, "hc",
    "an id devourer", 40, 4, 10, 300, 9, 0, "dEI",
    "a war spirit", 50, 6, 9, 450, 15, 0, "arR",
    "a Deodanth", 10, 2, 14, 90, 28, 0, "a",
    "a Siren", 10, 1, 14, 50, 13, 0, "HPV",
    "a deathmist", 15, 40, 2, 350, 3, 0, "Eih",
    "a guardian entity", 35, 26, 3, 300, 14, 0, "r",
    "a morphian", 20, 1, 16, 50, 23, 0, "hud",
    "a wind scavenger", 25, 3, 8, 75, 26, 0, "EHP",
    "a void walker", 50, 5, 21, 135, 27, 0, "sC",
    "a poltergiest", 20, 9, 4, 30, 19, 0, "PI",
    "an imp", 5, 1, 5, 24, 19, 0, "t",
    "a lunatic", 2, 1, 5, 24, 9, 0, " ",
    "a ghost of Ib", 10, 1, 8, 40, 14, 0, "Pi",
    "Abhoth", 40, 4, 10, 300, 10, 0, "R",
    "Cloudbeast", 40, 2, 24, 200, 10, 0, "Si",
    "a wamp", 10, 10, 4, 50, 18, 0, "dV",
    "an Abhoth offspring", 10, 1, 8, 25, 15, 0, "P",
    "an Atlach-Nacha", 18, 3, 8, 50, 17, 0, "VW",
    "a mist spider", 14, 3, 6, 40, 14, 0, "PW",
    "a leng spider", 16, 4, 6, 45, 16, 0, "pW",
    "a minion of Karakal", 25, 6, 34, 75, 20, 0, "Er",
    "a basilisk", 70, 1, 100, 50, 8, 0, "ApVd",
    "a nightrider", 13, 2, 20, 150, 14, 0, "uHa",
    "a fireworm", 15, 6, 4, 90, 11, 0, "a",
    "a Great One", 16, 2, 30, 150, 17, 0, "PchR",
    "a gug", 20, 5, 16, 100, 13, 0, "a",
    "larva of the Outer Gods",35, 4, 20, 100, 14, 0, "RPH"};

struct spell_table spell[NSPELLS] = {
    "Return to Earth", 10, RETURN, 0, 0,
    "ritual of Finding", 15, FINDING, 0, 0,
    "ritual of Food Creation", 5, FOOD, 0, 0,
    "ritual of Discovery", 10, DISCOVER, 0, 0,
    "ritual of Healing", 25, HEALING, 0, 0,
    "ritual of Identify", 15, IDENTIFY, 0, 0,
    "ritual of Isolation", 20, ISOLATION, 0, 0,
    "ritual of Longevity", 5, LONGEVITY, 0, 0,
    "ritual of Mythos", 20, MYTHOS, 0, 0,
    "ritual of Regeneration", 10, REGENERATION, 0, 0,
    "ritual of Sacrifice", 15, SACRIFICE, 0, 0,
    "ritual of Sanctuary", 10, SANCTUARY, 0, 0,
    "ritual of Solitude", 5, SOLITUDE, 0, 0,
    "ritual of Stealth", 15, STEALTH, 0, 0,
    "ritual of Teleport", 20, TELEPORT, 0, 0,
    "spell of Berserker", 5, BERSERKER, 1, 0,
    "spell of Blast", 20, BLAST, 1, 0,
    "spell of Cleansing", 10, CLEANSING, 1, 0,
    "spell of Corrupt", 20, CORRUPT, 1, 0,
    "spell of Escape", 20, ESCAPE, 1, 0,
    "spell of Excel", 20, EXCEL, 1, 0,
    "spell of Holy Aura", 5, HOLY_AURA, 1, 0,
    "spell of Immunity", 10, IMMUNITY, 1, 0,
    "spell of Invisibility", 10, INVISIBILITY, 1, 0,
    "spell of Judgement", 30, JUDGEMENT, 1, 0,
    "spell of Protection", 10, PROTECTION, 1, 0,
    "spell of Vampirism", 20, VAMPIRE, 1, 0,
    "spell of Wither", 20, WITHER, 1, 0};

int highlist = 10;

char death[MAXPATH];
char tmp[MAXPATH];

int pc, st_pos, mpd;
int cpc[NCITIES];
int tpc[NCITIES];
int cp[NCITIES];
int spl[NSPELLS];
int object[NOBJECTS];
int sign_index[NUMBERSIGNS];
