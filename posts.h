#ifndef POSTS_H
#define POSTS_H

#include "users.h"

typedef struct post_t post_t;
struct post_t {
	int id;
	char *title;
	int user_id;
	tree_t *events;
	int likes;
	int *user_likes;
};

#define FINGERPRINT 6666666

/**
 * Function that handles the calling of every command from POSTS
 *
 * Please add any necessary parameters to the functions
*/
void handle_input_posts(char *input, post_t *post_manager, int *psize, int *idx);

void free_post(post_t *root);

void print_reposts(post_t *root, int *task);

#endif // POSTS_H
