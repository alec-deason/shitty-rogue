#ifndef GAME_H
#define GAME_H


#define TICKS_PER_TURN 1000
#define MESSAGE_LENGTH 200
extern int keyboard_x, keyboard_y;
extern char message_banner[MESSAGE_LENGTH];

void print_message(char *msg);

#endif
