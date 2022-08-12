#ifndef OBJECTS_H
#define OBJECTS_H

void displayobj(int type);
int selectunknown(void);
int getitem(void);
void lose_item(char *thief);
int get_benefit(void);
void touch_sign(char c);
void useobj(void);
void drink(void);
void wield(void);
int lookup(int flag);
int did_find(int level);
void search(int rec, int power);
void getpipe(void);
void getaxe(void);
void robcorpse(int m);

#endif
