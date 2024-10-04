/**
 * Copyright 2024 Toma-Ioan Dumitrescu
 * 
 * PROGRAM ENTRY POINT
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "users.h"
#include "friends.h"
#include "posts.h"
#include "feed.h"

/**
 * Initializez every task based on which task we are running
*/
void init_tasks(void)
{
	#ifdef FRIENDS

	#endif

	#ifdef POSTS

	#endif

	#ifdef ALL

	#endif
}

void free_post(post_t *root)
{
	if (!root || !root->events || !root->events->root)
		return;

	node_t *events = root->events->root;

	// Recursive search over all subposts of the current post
	for (int i = 0; i < events->size; i++) {
		post_t *current = (post_t *)events->children[i]->data;
		free_post(current);

		free(current);
		free(events->children[i]);
	}

	free(events->children);
	free(events);
	free(root->events);
}

void free_data(list_graph_t *network, post_t *post_manager, int psize)
{
	// Free the network
	lg_free(network);

	// Free the post manageer
	for (int i = 0; i < psize; i++) {
		free_post(&post_manager[i]);
		free(post_manager[i].title);
	}

	free(post_manager);
}

/**
 * Entrypoint of the program, compiled with different defines for each task
*/
int main(void)
{
	init_users();

	init_tasks();

	list_graph_t *network = lg_create(MAX_USERS);

	post_t *post_manager = calloc(MAX_USERS, sizeof(post_t));
	if (!post_manager) {
		printf("Calloc failed!\n");
		exit(1);
	}

	int psize = 0, idx = 1;

	char *input = (char *)malloc(MAX_COMMAND_LEN);
	while (1) {
		char *command = fgets(input, MAX_COMMAND_LEN, stdin);

		// If fgets returns null, we reached EOF
		if (!command)
			break;

		#ifdef FRIENDS
		handle_input_friends(input, network);
		#endif

		#ifdef POSTS
		handle_input_posts(input, post_manager, &psize, &idx);
		#endif

		#ifdef ALL
		handle_input_feed(input);
		#endif
	}

	free_users();
	free(input);
	free_data(network, post_manager, psize);

	return 0;
}
