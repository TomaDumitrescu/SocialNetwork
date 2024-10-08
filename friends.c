#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "friends.h"

void add_friendship(char *friend1, char *friend2, list_graph_t *network)
{
	int id1 = get_user_id(friend1), id2 = get_user_id(friend2);

	lg_add_edge(network, id1, id2);
	lg_add_edge(network, id2, id1);

	printf("Added connection %s - %s\n", friend2, friend1);
}

void remove_friendship(char *friend1, char *friend2, list_graph_t *network)
{
	int id1 = get_user_id(friend1), id2 = get_user_id(friend2);

	lg_remove_edge(network, id1, id2);
	lg_remove_edge(network, id2, id1);

	printf("Removed connection %s - %s\n", friend2, friend1);
}

void calculate_distance(char *friend1, char *friend2, list_graph_t *network)
{
	int id1 = get_user_id(friend1), id2 = get_user_id(friend2);
	int *v = calloc(MAX_USERS, sizeof(int));
	if (!v) {
		printf("Calloc failed!\n");
		exit(1);
	}

	// distance with one BFS
	int d = distance(id1, id2, network, v);
	if (d > 0)
		printf("The distance between %s - %s is %d\n", friend2, friend1, d);
	else
		printf("There is no way to get from %s to %s\n", friend2, friend1);

	free(v);
}

void suggestions(char *user, list_graph_t *network) {
	int id = get_user_id(user);

	/*
	 *	Algorithm:
	 *  - Mark all user's friends with -1 (avoiding user's friends)
	 *  - Iterate over users friends and find other friends,
	 * marking them with 1 (avoiding duplicates)
	 *  - Add the found friends to a list if they are not marked
	 * with 1 or -1
	 */

	int *v = calloc(MAX_USERS, sizeof(int));
	if (!v) {
		printf("Calloc failed!");
		exit(1);
	}

	ll_node_t *node = network->neighbors[id]->head;
	while (node) {
		int neighbor = *(int *)node->data;
		v[neighbor] = -1;

		node = node->next;
	}

	int *suggestions = calloc(MAX_USERS, sizeof(int)), num = 0;
	if (!suggestions) {
		printf("Calloc failed!");
		exit(1);
	}

	node = network->neighbors[id]->head;
	while (node) {
		int neighbor = *(int *)node->data;

		ll_node_t *deep_search = network->neighbors[neighbor]->head;
		while (deep_search) {
			int suggestion = *(int *)deep_search->data;
			if (v[suggestion] != 1 && v[suggestion] != -1) {
				// O(1) list add_first
				suggestions[num++] = suggestion;
				v[suggestion] = 1;
			}

			// don't mark user's friends as visited
			if (v[suggestion] != -1)
				v[suggestion] = 1;

			deep_search = deep_search->next;
		}

		node = node->next;
	}

	free(v);

	qsort(suggestions, num, sizeof(int), compare);

	if (num > 1)
		printf("Suggestions for %s:\n", user);

	for (int i = 0; i < num; i++) {
		if (suggestions[i] == id)
			continue;
		char *name = get_user_name(suggestions[i]);
		printf("%s\n", name);
	}

	if (num <= 1)
		printf("There are no suggestions for %s\n", user);

	free(suggestions);
}

void common_friends(char *user1, char *user2, list_graph_t *network)
{
	int id1 = get_user_id(user1), id2 = get_user_id(user2);

	int *friends1 = calloc(MAX_USERS, sizeof(int)), num1 = 0;
	int *friends2 = calloc(MAX_USERS, sizeof(int)), num2 = 0;

	if (!friends1 || !friends2) {
		printf("Calloc failed!\n");
		exit(1);
	}

	ll_node_t *search = network->neighbors[id1]->head;
	while (search) {
		int neighbor = *(int *)search->data;
		friends1[num1++] = neighbor;

		search = search->next;
	}

	search = network->neighbors[id2]->head;
	while (search) {
		int neighbor = *(int *)search->data;
		friends2[num2++] = neighbor;

		search = search->next;
	}

	qsort(friends1, num1, sizeof(int), compare);
	qsort(friends2, num2, sizeof(int), compare);

	bool exist_common = false;
	int itr1 = 0, itr2 = 0;
	while (itr1 < num1 && itr2 < num2) {
		if (friends1[itr1] == friends2[itr2]) {
			if (!exist_common)
				printf("The common friends between %s and %s are:\n", user2, user1);

			exist_common = true;
			printf("%s\n", get_user_name(friends1[itr1]));
			itr1++, itr2++;
		} else if (friends1[itr1] < friends2[itr2]) {
			itr1++;
		} else {
			itr2++;
		}
	}

	if (!exist_common)
		printf("No common friends for %s and %s\n", user2, user1);

	free(friends1);
	free(friends2);
}

int friends(char *user, list_graph_t *network, bool print, int user_id)
{
	int id, num = 0;
	if (user_id == -1)
		id = get_user_id(user);
	else
		id = user_id;

	ll_node_t *search = network->neighbors[id]->head;
	while (search) {
		num++;
		search = search->next;
	}

	if (print)
		printf("%s has %d friends\n", user, num);

	return num;
}

void popular(char *user, list_graph_t *network)
{
	int id = get_user_id(user);
	int popular_friends = friends(user, network, false, id);
	int popular_id = id;

	ll_node_t *search = network->neighbors[id]->head;
	while (search) {
		int neighbor = *(int *)search->data;

		int n_friends = friends(user, network, false, neighbor);
		if (n_friends > popular_friends) {
			popular_id = neighbor;
			popular_friends = n_friends;
		}

		search = search->next;
	}

	char *popular_name = get_user_name(popular_id);
	if (popular_id == id)
		printf("%s is the most popular\n", user);
	else
		printf("%s is the most popular friend of %s\n", popular_name, user);
}

void handle_input_friends(char *input, list_graph_t *network)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd)
		return;

	// Commands menu for user network
	if (!strcmp(cmd, "add"))
		add_friendship(strtok(NULL, "\n "), strtok(NULL, "\n "), network);
	else if (!strcmp(cmd, "remove"))
		remove_friendship(strtok(NULL, "\n "), strtok(NULL, "\n "), network);
	else if (!strcmp(cmd, "suggestions"))
		suggestions(strtok(NULL, "\n "), network);
	else if (!strcmp(cmd, "distance"))
		calculate_distance(strtok(NULL, "\n "), strtok(NULL, "\n "), network);
	else if (!strcmp(cmd, "common"))
		common_friends(strtok(NULL, "\n "), strtok(NULL, "\n "), network);
	else if (!strcmp(cmd, "friends"))
		friends(strtok(NULL, "\n "), network, true, -1);
	else if (!strcmp(cmd, "popular"))
		popular(strtok(NULL, "\n "), network);

	free(commands);
}
