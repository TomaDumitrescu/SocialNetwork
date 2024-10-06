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
	post_manager[*psize].likes = 0;
	post_manager[*psize].user_likes = calloc(MAX_USERS / 2, sizeof(int));
	if (!post_manager[*psize].user_likes) {
		printf("Calloc failed!\n");
		exit(1);
	}

	post_manager[*psize].events = NULL;

	(*psize)++;

	printf("Created %s for %s\n", title, user);
}

// Searches in the posts tree of a post, the repost with repost_id
post_t *search_repost(int repost_id, post_t *post)
{
	if (repost_id == -1)
		return post;

	if (repost_id != -1 && (!post->events || !post->events->root))
		return NULL;

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
	new_post->user_likes = calloc(MAX_USERS / 2, sizeof(int));
	if (!new_post->user_likes) {
		printf("Calloc failed!\n");
		exit(1);
	}

	new_post->likes = 0;

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

	if (repost_id == -1) {
		char *user = get_user_name(potential->user_id);
		printf("%s - Post by %s\n", potential->title, user);
	}

	post_t *target = search_repost(repost_id, potential);
	if (target && repost_id != -1)
		printf("Repost #%d by %s\n", target->id, get_user_name(target->user_id));
	print_reposts(target);
}

post_t *find_common(post_t *root, int r_id1, int r_id2)
{
	if (!root)
		return NULL;

	if (root->id == r_id1 || root->id == r_id2)
		return root;

	if (!root->events || !root->events->root)
		return NULL;

	post_t *potential = NULL, *result = NULL;
	node_t *events = root->events->root;

	int num_sides = 0;

	for (int i = 0; i < events->size; i++) {
		post_t *current = (post_t *)events->children[i]->data;
		potential = find_common(current, r_id1, r_id2);
		if (potential) {
			result = potential;
			num_sides++;
		}
	}

	if (num_sides >= 2)
		result = root;

	return result;
}

void common_repost(int p_id, int r_id1, int r_id2, post_t *post_manager,
				   int *psize)
{
	post_t *root_common = NULL;
	for (int i = 0; i < *psize; i++)
		if (p_id == post_manager[i].id) {
			root_common = &post_manager[i];
		}

	post_t *first_common = find_common(root_common, r_id1, r_id2);

	printf("The first common repost of %d and %d is %d\n",
			r_id1, r_id2, first_common->id);
}

void like(char *user, int p_id, int r_id, post_t *post_manager, int *psize)
{
	char post_word[] = "post", repost_word[] = "repost";
	char *word = (r_id == -1)? post_word : repost_word;

	int id = get_user_id(user);

	post_t *target_post = NULL, *post;
	for (int i = 0; i < *psize; i++) {
		if (post_manager[i].id == p_id) {
			target_post = &post_manager[i];
			post = &post_manager[i];
			break;
		}
	}

	post_t *target = search_repost(r_id, target_post);

	int cnt = 0, len = target->likes, idx = 0;
	while (cnt < len) {
		if (target->user_likes[idx] == id) {
			target->user_likes[idx] = 0;
			target->likes--;
			printf("User %s unliked %s %s\n", user, word, post->title);
			return;
		}

		if (target->user_likes[idx] != 0)
			cnt++;
		idx++;
	}

	cnt = 0, idx = 0;
	while (true) {
		if (target->user_likes[idx] != 0) {
			cnt++;
		} else {
			target->user_likes[idx] = id;
			break;
		}
		idx++;
	}

	target->likes++;
	printf("User %s liked %s %s\n", user, word, post->title);
}

void get_likes(int p_id, int r_id, post_t *post_manager, int *psize)
{
	post_t *target_post = NULL;
	for (int i = 0; i < *psize; i++) {
		if (post_manager[i].id == p_id) {
			target_post = &post_manager[i];
			break;
		}
	}

	post_t *target = search_repost(r_id, target_post);
	if (r_id == -1)
		printf("Post %s has %d likes\n", target->title, target->likes);
	else
		printf("Repost #%d has %d likes\n", target->id, target->likes);
}

void compare_reposts(post_t *post, int *likes, int *id)
{
	if (!post)
		return;

	if (post->likes == *likes && post->id < *id)
		*id = post->id;

	if (post->likes > *likes)
		*id = post->id;

	if (!post->events || !post->events->root)
		return;

	node_t *events = post->events->root;
	for (int i = 0; i < events->size; i++) {
		post_t *next = (post_t *)events->children[i]->data;
		compare_reposts(next, &(*likes), &(*id));
	}
}

void ratio(int p_id, post_t *post_manager, int *psize)
{
	post_t *target = NULL;
	for (int i = 0; i < *psize; i++)
		if (post_manager[i].id == p_id) {
			target = &post_manager[i];
			break;
		}

	int ratio_repost = -1, likes = target->likes;
	compare_reposts(target, &likes, &ratio_repost);

	if (ratio_repost == -1)
		printf("The original post is the highest rated\n");
	else
		printf("Post %d got ratio'd by repost %d\n", p_id, ratio_repost);
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
	} else if (!strcmp(cmd, "common-repost")) {
		int p_id = atoi(strtok(NULL, "\n ")), r_id1 = atoi(strtok(NULL, "\n "));
		int r_id2 = atoi(strtok(NULL, "\n "));
		common_repost(p_id, r_id1, r_id2, post_manager, &(*psize));
	} else if (!strcmp(cmd, "like")) {
		char *name = strtok(NULL, "\n "), *p_id = strtok(NULL, "\n ");
		char *r_id = strtok(NULL, "\n ");
		int r_idnum = (r_id)? atoi(r_id) : -1;
		like(name, atoi(p_id), r_idnum, post_manager, &(*psize));
	} else if (!strcmp(cmd, "ratio")){
		ratio(atoi(strtok(NULL, "\n ")), post_manager, &(*psize));
	} else if (!strcmp(cmd, "delete"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "get-likes")) {
		char *p_id = strtok(NULL, "\n "), *r_id = strtok(NULL, "\n ");
		int r_idnum = (r_id)? atoi(r_id) : -1;
		get_likes(atoi(p_id), r_idnum, post_manager, &(*psize));
	} else if (!strcmp(cmd, "get-reposts")) {
		char *p_id = strtok(NULL, "\n "), *r_id = strtok(NULL, "\n ");
		int r_idnum = (r_id)? atoi(r_id) : -1;
		get_reposts(atoi(p_id), r_idnum, post_manager, &(*psize));
	} else if (!strcmp(cmd, "get-likes"))
		(void)cmd;
		// TODO: Add function

	free(commands);
}
