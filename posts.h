#ifndef POSTS_H
#define POSTS_H

#include "users.h"

typedef struct post_t post_t;
struct post_t {
	int id;
	char *title;
	int user_id;
	tree_t *events;
};

/**
 * Function that handles the calling of every command from POSTS
 *
 * Please add any necessary parameters to the functions
*/
void handle_input_posts(char *input, post_t *post_manager, int *psize, int *idx);

#endif // POSTS_H
