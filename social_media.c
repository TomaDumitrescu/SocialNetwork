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

/**
 * Entrypoint of the program, compiled with different defines for each task
*/
int main(void)
{
	init_users();

	init_tasks();

	list_graph_t *network = lg_create(MAX_USERS);

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
		handle_input_posts(input, network);
		#endif

		#ifdef ALL
		handle_input_feed(input, network);
		#endif
	}

	lg_free(network);
	free_users();
	free(input);

	return 0;
}
