### Copyright 2024 Toma-Ioan Dumitrescu

### Description

Social network simulator which manages user networks, posts
and feed with no valgrind memory errors (also defensive programming).
The saved users are in users.db, and their relationships via a graph.
Posts are stored in an array of trees, because of the nature of
features this app provides and because of the good hierarchy of posts
and reposts offered by this design. Feed commands use the
user graph and the vector of tree-posts. The mini-app was tested using
27 files of type in, out, ref (expected output) with file diff.

See the commands menu in the sources where it's strcmp to
test the program by yourself.

### Implementation and functionalities

The entry point of the program is in social_media.c which offers
an input handling based on the created binaries that come with
some symbols, so that specific commands are processed by the
specialized C modules.

1) ADD FRIEND: an edge between two user nodes in the network

2) RM FRIEND: remove the edge between the two user nodes

3) FIND DISTANCE: done with one BFS, because of equal weight

4) SUGGESTIONS: use a visit vector so that all friends of the
targeted user (including him) are marked as visit, and only the
not marked friends of the user friends are shown

5) COMMON FRIENDS: store the friends of the user1 in friends1 and
the friends of the user2 in friends2. Sort increasingly friends1
and friends2. Consider two pointers, one at friends1[0] and one
at friends2[0]. Moving them according to the comparisons will
provide the list of common friends.

6) NUMBER OF FRIENDS: traverse the neighbors of the user_id node
and count them

7) MOST POPULAR: traverse the friends of the user and the user
and count their connections. Show the user with the biggest
number of friends (in equality case, the targeted user name
is printed).

8) CREATE POST: also a way to create a user, put the post
in the vector

9) REPOST: create a repost at an existing post, put the
repost in a vector post element (in post_manager)

10) FIRST-COMMON-REPOST: at every node, find how many branches
contain the targeted reposts. If there is more than one branch,
then the respective node is the first-common-repost. Also, go
downwards recursively on the branch with the targeted reposts

11) LIKE: like or dislike functionality (fill the first zero in
the likes vector of the post with the user who liked the post or
put a zero in the likes vector at the element with the id of
the user who currently dislikes)

12) RATIO: verify if the original post has more likes than the
reposts or which repost ratio'd the original post (in equality case,
the smallest repost id is considered)

13) DELETE POST/REPOST: delete the post/repost and its events from
the memory in real time, since the (re)posts ar dynmically allocated

14) GET LIKES: print the number of likes from the likes vector
(number of nonzero elements)

15) GET REPOSTS: print the post and its reposts or the repost and
its reposts, by traversing recursively the root post and its event nodes.

16) FEED: print the most recent N posts of the targeted user friends, by
traversing post_manager in reversed order (to assure the most recent ones)
and verify if the post has the user_id in the friends list of the targeted
user.

17) VIEW PROFILE: prints all posts and reposts of a user, by traversing
the post_manager vector and for each root-post, traverse recursively its
children and check if user_id of a child equals get_id(user_name)

18) FRIENDS THAT REPOSTED: for every repost id of a post/repost, verify
if that repost->user_id is in the friends_id list of the targeted user
and print the name of the users using the function get_name_by_id(user_name)

19) MOST LIKED POST: iterate over the posts and find the one with the
maximum number of likes

20) LONGEST COMMON-GROUP: the group of friends of an user with maximum nodes
where all people are friends with all other (longest clique). Bron Kerbosch
is applied first to iterate over the maximal cliques and find the maximum
number of nodes a maximal clique can have. Apply again Bron Kerbosch to
print the maximal clique with that maximum number of nodes. A weakness
can be a lot of memory allocation and brute force (backtracking-like). An
optimization for this can be to use the Bron Kerbosch algorithm with pivot.

### Bibliography
Bron Kerbosch algorithm, without pivoting for the longest clique
