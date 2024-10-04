#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "posts.h"

void create_post(char *user, char *title, post_t *post_manager,
				 int *psize, int *idx)
{
	int user_id = add_user(user);

	post_manager[*psize].id = (*idx)++;
	post_manager[*psize].title = strdup(title);
	post_manager[*psize].user_id = user_id;
	post_manager[*psize].events = NULL;

	(*psize)++;

	printf("Created %s for %s\n", title, user);
}

// Searches in the posts tree of a post, the repost with repost_id
post_t *search_repost(int repost_id, post_t *post)
{
	if (repost_id == -1 || !post->events || !post->events->root)
		return post;

	node_t *events = post->events->root;
	for (int i = 0; i < events->size; i++) {
		post_t *potential = (post_t *)events->children[i]->data;
		if (potential->id == repost_id)
			return potential;
	}

	// Recursive search over all subposts of the current post
	for (int i = 0; i < events->size; i++) {
		post_t *potential = (post_t *)events->children[i]->data;
		post_t *deep_potential = search_repost(repost_id, potential);
		if (deep_potential)
			return deep_potential;
	}

	return NULL;
}

void repost(char *user, int post_id, int repost_id, post_t *post_manager,
			int *psize, int *idx)
{
	if (post_id == repost_id)
		repost_id = -1;

	post_t *target_post = NULL;
	for (int i = 0; i < *psize; i++)
		if (post_manager[i].id == post_id) {
			target_post = &post_manager[i];
			break;
		}

	post_t *target_repost = search_repost(repost_id, target_post);
	if (!target_repost->events) {
		target_repost->events = malloc(sizeof(*target_repost->events));
		if (!target_repost->events) {
			printf("Malloc failed!\n");
			exit(1);
		}

		target_repost->events->root = malloc(sizeof(*target_repost->events->root));
		if (!target_repost->events->root) {
			printf("Malloc failed!\n");
			exit(1);
		}

		node_t *root = target_repost->events->root;
		root->children = malloc(REALLOC_REF * sizeof(node_t *));
		if (!root->children) {
			printf("Malloc failed\n");
			exit(1);
		}

		root->size = 0, root->num_reallocs = 1;
	}

	node_t *root = target_repost->events->root;
	if (root->size + 1 >= root->num_reallocs * REALLOC_REF) {
		root->children = realloc(root->children,
			(++root->num_reallocs) * sizeof(node_t *));
		if (!root->children) {
			printf("Malloc failed\n");
			exit(1);
		}
	}

	root->children[root->size] = malloc(sizeof(*root->children[root->size]));
	if (!root->children[root->size]) {
		printf("Malloc failed!\n");
		exit(1);
	}

	root->children[root->size]->size = 0;
	root->children[root->size]->num_reallocs = 0;
	root->children[root->size]->children = NULL;

	post_t *new_post = malloc(sizeof(*new_post));
	if (!new_post) {
		printf("Malloc failed!\n");
		exit(1);
	}

	new_post->id = (*idx)++;
	new_post->title = NULL;
	new_post->user_id = get_user_id(user);
	new_post->events = NULL;

	root->children[root->size]->data = (void *)new_post;

	root->size++;

	printf("Created repost #%d for %s\n", *idx - 1, user);
}

void print_reposts(post_t *root)
{
	if (!root || !root->events || !root->events->root)
		return;

	node_t *events = root->events->root;

	// Recursive search over all subposts of the current post
	for (int i = 0; i < events->size; i++) {
		post_t *current = (post_t *)events->children[i]->data;
		char *user = get_user_name(current->user_id);
		printf("Repost #%d by %s\n", current->id, user);
		print_reposts(current);
	}
}

void get_reposts(int post_id, int repost_id, post_t *post_manager,
				 int *psize)
{
	post_t *potential = NULL;
	for (int i = 0; i < *psize; i++)
		if (post_id == post_manager[i].id) {
			potential = &post_manager[i];
		}

	char *user = get_user_name(potential->user_id);
	printf("%s - Post by %s\n", potential->title, user);

	post_t *target = search_repost(repost_id, potential);
	print_reposts(target);
}

void handle_input_posts(char *input, post_t *post_manager, int *psize, int *idx)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!cmd)
		return;

	if (!strcmp(cmd, "create")) {
		char *name = strtok(NULL, "\n "), *title = strtok(NULL, "\n");
		create_post(name, title, post_manager, &(*psize), &(*idx));
	} else if (!strcmp(cmd, "repost")) {
		char *name = strtok(NULL, "\n "), *p_id = strtok(NULL, "\n ");
		char *r_id = strtok(NULL, "\n ");
		int r_idnum = (r_id)? atoi(r_id) : -1;
		repost(name, atoi(p_id), r_idnum, post_manager, &(*psize), &(*idx));
	} else if (!strcmp(cmd, "common-repost"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "like"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "ratio"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "delete"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "get-likes"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "get-reposts")) {
		char *p_id = strtok(NULL, "\n "), *r_id = strtok(NULL, "\n ");
		int r_idnum = (r_id)? atoi(r_id) : -1;
		get_reposts(atoi(p_id), r_idnum, post_manager, &(*psize));
	} else if (!strcmp(cmd, "get-likes"))
		(void)cmd;
		// TODO: Add function

	free(commands);
}
