#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "friends.h"
#include "users.h"

void add_friendship(char *friend1, char *friend2, list_graph_t *network)
{
	int id1 = get_user_id(friend1), id2 = get_user_id(friend2);

	lg_add_edge(network, id1, id2);
	lg_add_edge(network, id2, id1);

	printf("Added connection %s - %s\n", friend1, friend2);
}

void remove_friendship(char *friend1, char *friend2, list_graph_t *network)
{
	int id1 = get_user_id(friend1), id2 = get_user_id(friend2);

	lg_remove_edge(network, id1, id2);
	lg_remove_edge(network, id2, id1);

	printf("Removed connection %s - %s\n", friend1, friend2);
}

void calculate_distance(char *friend1, char *friend2, list_graph_t *network)
{
	int id1 = get_user_id(friend1), id2 = get_user_id(friend2);
	int *v = calloc(MAX_USERS, sizeof(int));
	if (!v) {
		printf("Calloc failed!\n");
		exit(1);
	}

	int d = distance(id1, id2, network, v);
	if (d > 0)
		printf("The distance between %s - %s is %d\n", friend1, friend2, d);
	else
		printf("There is no way to get from %s to %s\n", friend1, friend2);
}

void handle_input_friends(char *input, list_graph_t *network)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd)
		return;

	if (!strcmp(cmd, "add"))
		add_friendship(strtok(cmd, NULL), strtok(cmd, NULL), network);
	else if (!strcmp(cmd, "remove"))
		remove_friendship(strtok(cmd, NULL), strtok(cmd, NULL), network);
	else if (!strcmp(cmd, "suggestions"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "distance"))
		calculate_distance(strtok(cmd, NULL), strtok(cmd, NULL), network);
	else if (!strcmp(cmd, "common"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "friends"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "popular"))
		(void)cmd;
		// TODO: Add function

	free(commands);
}
