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

void view_profile(char *user, post_t *post_manager, int psize)
{
	int id = get_user_id(user);

	// Firstly, print the user posts in order
	for (int i = 0; i < psize; i++) {
		post_t *post = &post_manager[i];
		if (post->user_id == id)
			printf("Posted: %s\n", post->title);
	}

	for (int i = 0; i < psize; i++) {
		post_t *post = &post_manager[i];

		int info = id;
		print_reposts(post, &info);

		// A repost by this user was found
		if (info == FINGERPRINT)
			printf("Reposted: %s\n", post->title);
	}
}

void friends_repost(char *user, int p_id, list_graph_t *network,
					post_t *post_manager, int psize)
{
	int id = get_user_id(user), cnt = 0;

	int *ids = calloc(MAX_USERS / 2, sizeof(int));
	if (!ids) {
		printf("Calloc failed!\n");
		exit(1);
	}

	// Determining the friends of the current user
	ll_node_t *search = network->neighbors[id]->head;
	while (search) {
		int neighbor = *(int *)search->data;
		ids[cnt++] = neighbor;
		search = search->next;
	}

	for (int i = 0; i < psize; i++) {
		post_t *post = &post_manager[i];
		if (post->id != p_id)
			continue;

		for (int j = 0; j < cnt; j++) {
			int info = ids[j];
			print_reposts(post, &info);

			// A repost by this user was found
			if (info == FINGERPRINT)
				printf("%s\n", get_user_name(ids[j]));
		}

		break;
	}

	free(ids);
}

void print_clique(int *r, int rs, char *user)
{
	printf("The closest friend group of %s is:\n", user);
	qsort(r, rs, sizeof(int), compare);
	for (int i = 0; i < rs; i++)
		printf("%s\n", get_user_name(r[i]));
}

void bron_kerbosch(int *r, int *p, int *x, int rs, int ps, int xs, int cnt,
				   list_graph_t *network, char *user, int *lmax, bool print)
{
	// P = X = void set
	if (ps == 0 && xs == 0) {
		if (print && rs == *lmax)
			print_clique(r, rs, user);
		else if (*lmax < rs)
			*lmax = rs;
		free(r), free(p), free(x);
		return;
	}

	// Recursive step
	int i = 0;
	while (i < ps) {
		int v = p[i];

		// alloc some memory for the new vectors
		int *new_r = calloc(cnt + 1, sizeof(int));
		int *new_p = calloc(cnt + 1, sizeof(int));
		int *new_x = calloc(cnt + 1, sizeof(int));
		if (!new_r || !new_p || !new_x) {
			printf("Calloc failed!\n");
			exit(1);
		}

		// new_R = R U {v}
		for (int j = 0; j < rs; j++)
			new_r[j] = r[j];

		new_r[rs] = v;

		// new_P = P intersected with N(v)
		int cnt_p = 0;
		for (int j = 0; j < ps; j++)
			if (lg_has_edge(network, p[j], v))
				new_p[cnt_p++] = p[j];

		// new_X = X intersected with N(v)
		int cnt_x = 0;
		for (int j = 0; j < xs; j++)
			if (lg_has_edge(network, x[j], v))
				new_x[cnt_x++] = x[j];

		bron_kerbosch(new_r, new_p, new_x, rs + 1, cnt_p, cnt_x, cnt,
						network, user, &(*lmax), print);

		// P = P \ {v}
		p[i] = p[--ps];

		// X = X U {v}
		x[xs++] = v;
	}

	free(r);
	free(p);
	free(x);
}

void common_group(char *user, list_graph_t *network)
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

	/* Clique algorithm for finding longest maximal clique
	 * number of nodes
	 */
	int *r = calloc(cnt + 1, sizeof(int));
	int *p = calloc(cnt + 1, sizeof(int));
	int *x = calloc(cnt + 1, sizeof(int));
	if (!r || !p || !x) {
		printf("Calloc failed!\n");
		exit(1);
	}

	
	for (int i = 0; i < cnt; i++)
		p[i] = ids[i];

	int lmax = 1;
	bron_kerbosch(r, p, x, 0, cnt, 0, cnt, network, user, &lmax, false);

	/* Clique algorithm for printing the longest clique using the
	 * lmax value found previously
	 */
	r = calloc(cnt + 1, sizeof(int));
	p = calloc(cnt + 1, sizeof(int));
	x = calloc(cnt + 1, sizeof(int));
	if (!r || !p || !x) {
		printf("Calloc failed!\n");
		exit(1);
	}

	for (int i = 0; i < cnt; i++)
		p[i] = ids[i];

	bron_kerbosch(r, p, x, 0, cnt, 0, cnt, network, user, &lmax, true);

	free(ids);
}

void handle_input_feed(char *input, list_graph_t *network,
						post_t *post_manager, int psize)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd)
		return;

	if (!strcmp(cmd, "feed")) {
		char *name = strtok(NULL, "\n "), *fsize = strtok(NULL, "\n ");
		feed(name, atoi(fsize), network, post_manager, psize);
	} else if (!strcmp(cmd, "view-profile")) {
		view_profile(strtok(NULL, "\n "), post_manager, psize);
	} else if (!strcmp(cmd, "friends-repost")) {
		char *name = strtok(NULL, "\n "), *p_id = strtok(NULL, "\n ");
		friends_repost(name, atoi(p_id), network, post_manager, psize);
	} else if (!strcmp(cmd, "common-group")) {
		common_group(strtok(NULL, "\n "), network);
	}

	free(commands);
}
