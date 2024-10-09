#ifndef FEED_H
#define FEED_H

#include "users.h"
#include "posts.h"

/**
 * Function that handles the calling of every command from ALL
 */
void handle_input_feed(char *input, list_graph_t *network,
                        post_t *post_manager, int psize);

#endif // FEED_H
