#ifndef FRIENDS_H
#define FRIENDS_H

#define MAX_COMMAND_LEN 500
#define MAX_PEOPLE 550

#include "users.h"

/**
 * Function that handles the calling of every command from FRIENDS
 */
void handle_input_friends(char *input, list_graph_t *network);

#endif // FRIENDS_H
