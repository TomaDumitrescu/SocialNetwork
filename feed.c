#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "feed.h"

void handle_input_feed(char *input, list_graph_t *network)
{
	char *commands = strdup(input);
	char *cmd = strtok(commands, "\n ");

	if (!network)
		printf("Compiler ignore -- Remove this");

	if (!cmd)
		return;

	if (!strcmp(cmd, "feed"))
		(void)cmd;
		// TODO: Add function
	else if (!strcmp(cmd, "view-profile"))
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
