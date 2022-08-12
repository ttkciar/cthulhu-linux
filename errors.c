#include "errors.h"
#include "cthulhu.h"
#include "init.h"
#include "graphics.h"
#include "end_game.h"

void buserr(int x) {
	STATE("HEY, I just had a bus error...");
	Quit(0);
}

void segerr(int x) {
	STATE("Hey, I just had a segmentation violation...");
	Quit(0);
}
