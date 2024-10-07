#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "feed.h"

void feed(char *user, int feed_size, list_graph_t *network,
		  post_t *post_manager, int psize)
{
	int id = get_user_id(user), cnt = 0;

	int *ids = calloc(MAX_USERS / 2, sizeof(int));
	if (!ids) {
		printf("Calloc failed!\n");
		exit(1);
	}

	ids[cnt++] = id;

	// Determining the friends of the current user
	ll_node_t *search = network->neighbors[id]->head;
	while (search) {
		int neighbor = *(int *)search->data;
		ids[cnt++] = neighbor;
		search = search->next;
	}

	// Most recent added at the end of post_manager,
	// so an inverse traverse is required
	for (int i = psize - 1; i >= 0; i--) {
		post_t *post = &post_manager[i];
		int user_id = post->user_id;
		bool in_group = false;

		for (int j = 0; j < cnt; j++)
			if (ids[j] == user_id) {
				in_group = true;
				break;
			}

		if (!in_group)
			continue;

		if (feed_size == 0)
			break;

		char *name = get_user_name(user_id);
		printf("%s: %s\n", name, post->title);

		// Only feed_size will be printed
		feed_size--;
	}

	free(ids);
}

void handle_input_feed(char *input, list_graph_t *network,
						post_t *post_manager, int psize, int idx)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd)
		return;

	if (!strcmp(cmd, "feed")) {
		char *name = strtok(NULL, "\n "), *fsize = strtok(NULL, "\n ");
		feed(name, atoi(fsize), network, post_manager, psize);
	} else if (!strcmp(cmd, "view-profile"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "friends-repost"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "common-groups"))
		(void)cmd;
		// TODO: Add function

	free(commands);
}
