#include "network.h"
#include <stdio.h>
#include <lcthw/stats.h>
#include <lcthw/tstree.h>
#include <lcthw/bstrlib.h>
#include "limits.h"
#include "float.h"

const char *help = "USAGE: <command> <name> [parameter]\n\
Available commands: create, sample, mean, dump, delete, list\n\
Parameter is only supplied for sample command\n\
EXAMPLE:\n\
create test\n\
sample test 10\n\
sample test 20\n\
mean test\n\
delete test\n\n";

/*bstring create = bsStatic("create");
bstring sample = bsStatic("sample");
bstring mean = bsStatic("mean");
bstring dump = bsStatic("dump");
bstring delete = bsStatic("delete");
bstring list = bsStatic("list");*/

const char *verbs_str = "create sample mean dump delete list";

short int check_input(struct bstrList *data)
{
    check((data->qty > 0) && (data->qty < 4), 
	    "Wrong number of parameters in input.");

    short int res = 0;
    bstring verb = data->entry[0];
    struct bstrList *verbs = NULL;
    verbs = bsplit(bfromcstr(verbs_str), ' ');

    int i = 0;
    for (i = 0; i < verbs->qty; i++) {
        res = (res || biseq(verb, verbs->entry[i]));
    }
    
    //res = (biseq(verb, create) || biseq(verb, sample) || biseq(verb, mean) ||
//		biseq(verb, dump) || biseq(verb, delete) || biseq(verb, list));

    /*if ((res == 1) && (biseq(verb, sample) == 0) && (data->qty != 2)) {
        res = 0;
    }*/

//    if ((biseq(verb, sample) == 1) && (data->qty != 3)) {
//        res = 0;
 //   }

    bstrListDestroy(verbs);

    return res;

error:
    return 0;
}

int count_slashes(bstring str)
{
    int i = 0;
    int count = 0;
    for (i = 0; i < blength(str); i++) {
	if (bdata(str)[i] == '/') {
	    count += 1;
	}
    }
    //printf("count: %d\n", count);
    
    return count;
}

struct bstrList *parse_url(bstring url)
{
    int i = 0;
    int count = count_slashes(url);

    struct bstrList *list = bstrListCreate();
    list->qty = count;
    list->mlen = count;
    list->entry = calloc(1, sizeof(bstring) * count);
    list->entry[0] = bstrcpy(url);

    int j = 1;
    i = blength(url) - 1;
    while (i != 0) {
        if (bdata(url)[i] == '/') {
	    i--;
	    list->entry[j] = blk2bstr(bdata(url), i+1);
	    j++;
	} else {
	    i--;
	}
    }

    /*for (i = 0; i < count; i++) {
        printf("%s\n", bdata(list->entry[i]));
    }*/

    return list;
}

TSTree *adjust_min_max(TSTree *tree, bstring child, bstring parent)
{
    DArray *children = TSTree_collect(tree, bdata(parent), blength(parent));
    Stats *stats = TSTree_search(tree, bdata(parent), blength(parent));
    double min = DBL_MAX;
    double max = 0;
    int child_level = count_slashes(child);
    int i = 0;
    for (i = 0; i < DArray_count(children); i++) {
	bstring cur = (bstring)DArray_get(children, i);
        if (count_slashes(cur) != child_level) {
	    continue;
	}
	if (biseq(cur, child) == 1) {
	    continue;
	}

	Stats *st = TSTree_search(tree, bdata(cur), blength(cur));
	if (st->min < min) {
	    min = st->min;
	}
	if (st->max > max) {
	    max = st->max;
	}

    }

    stats->min = min;
    stats->max = max;

    return tree;
}

TSTree *adjust_stats(TSTree *tree, bstring name)
{
    Stats *stats = TSTree_search(tree, bdata(name), blength(name));
    struct bstrList *parents = parse_url(name);
    int i = 0;
    if (parents->qty == 1) {
        return tree;
    }
    bstring parent = parents->entry[1];
    Stats *parent_stats = TSTree_search(tree, bdata(parent), blength(parent));

    parent_stats->sum -= stats->sum;
    parent_stats->sumsq -= stats->sumsq;
    parent_stats->n -= stats->n;

    if ((parent_stats->min == stats->min) || (parent_stats->max == stats->max)) {
	for (i = 0; i < parents->qty - 1; i++) {
            tree = adjust_min_max(tree, parents->entry[i], parents->entry[i+1]);
	}
    }

    return tree;
}

TSTree *execute(TSTree *tree, struct bstrList *data, char **out_buf)
{
    bstring create = bfromcstr("create");
    bstring sample = bfromcstr("sample");
    bstring mean = bfromcstr("mean");
    bstring dump = bfromcstr("dump");
    bstring delete = bfromcstr("delete");
    bstring list = bfromcstr("list");

    bstring verb = data->entry[0];
    int i = 0;

    if (biseq(verb, list) == 1) {
	bstring running = bfromcstr("");
	DArray *names = TSTree_collect_keys(tree, running);
	sprintf(*out_buf, "Statistics available on server:\n");
	for (i = 0; i < DArray_count(names); i++) {
	    sprintf(*out_buf, "%s %s\n", *out_buf, bdata((bstring)DArray_get(names, i)));
	}
	DArray_clear_destroy(names);
        bdestroy(running);
	return tree;
    }

    bstring name = data->entry[1];

    struct bstrList *parents = parse_url(name);
    for (i = 1; i < parents->qty; i++) {
        if (!TSTree_search(tree, bdata(parents->entry[i]), blength(parents->entry[i]))) {
	    sprintf(*out_buf, "Parent statistic %s not found, create it first!\n", bdata(parents->entry[i]));
	    return tree;
	}
    }
    
    Stats *stats = TSTree_search(tree, bdata(name), blength(name));

    if (biseq(verb, create) == 1) {
	if (stats) {
	    sprintf(*out_buf, "Statistic %s already exists on server!\n", bdata(name));
	    return tree;
	}
        
	stats = Stats_create();
	tree = TSTree_insert(tree, bdata(name), blength(name), stats);
	sprintf(*out_buf, "Created statistic %s\n", bdata(name));
    } else { 
	if (!stats) {
	    sprintf(*out_buf, "Statistic %s not found on a server\n", bdata(name));
	    return tree;
	}

	if (biseq(verb, sample) == 1) {
	    double s = atof(bdata(data->entry[2]));

	    for (i = 1; i < parents->qty; i++) {
		Stats *cur = TSTree_search(tree, bdata(parents->entry[i]), blength(parents->entry[i]));
		Stats_sample(cur, s);
	    }

	    Stats_sample(stats, s);
	    sprintf(*out_buf, "Added sample %.2f to statistic %s\n", s, bdata(name));
	}

        if (biseq(verb, mean) == 1) {
	    double mean = Stats_mean(stats);
	    sprintf(*out_buf, "Mean for statistic %s is %.4f\n", bdata(name), mean);
        }

        if (biseq(verb, dump) == 1) {
            Stats_dump_buf(stats, *out_buf); 
	}

        if (biseq(verb, delete) == 1) {
	    DArray *substats = TSTree_collect(tree, bdata(name), blength(name));
	    if (substats) {
		//printf("count: %d\n", DArray_count(substats));
		/*for (i = 0; i < DArray_count(substats); i++) {
		    printf("%s\n", bdata((bstring)DArray_get(substats, i)));
		}*/
		for (i = 0; i < DArray_count(substats) - 1; i++) {
		    bstring cur = (bstring)DArray_get(substats, i);
		    TSTree_delete(tree, bdata(cur), blength(cur), NULL);
		    bdestroy(cur);
		}
		bdestroy((bstring)DArray_get(substats, DArray_count(substats)-1));
		DArray_destroy(substats);
	    }

	    tree = adjust_stats(tree, name);

	    TSTree_delete(tree, bdata(name), blength(name), NULL);
	    sprintf(*out_buf, "Deleted statistic %s from the server\n", bdata(name));
        }
    }

    bstrListDestroy(parents);
    bdestroy(create);
    bdestroy(sample);
    bdestroy(mean);
    bdestroy(dump);
    bdestroy(delete);
    bdestroy(list);

    return tree;
}

int main(int argc, char *argv[])
{
    int socket = 0;
    int connfd = 0;
    char *in_buf = NULL;
    char *out_buf = NULL;
    bstring input = NULL;
    struct bstrList *data = NULL;
    TSTree *tree = NULL;

    socket = create_socket(7789);
    check(socket != -1, "Failed to open a socket.");

    in_buf = calloc(1, 1025);
    out_buf = calloc(1, 1025);

    int rc = listen(socket, 10);
    check(rc == 0, "Failed to listen on socket.");

    while (1) {
        connfd = accept(socket, (struct sockaddr_in *)NULL, NULL);
	check(connfd != -1, "Failed to accept incoming connection.");

	while (recv(connfd, in_buf, 1024, 0) > 0) {
	    printf("Received: %s\n", in_buf);

	    input = bfromcstr(strtok(in_buf, "\n"));
	    data = bsplit(input, ' ');
	    short int input_correct = check_input(data);
	    if (input_correct) {
		tree = execute(tree, data, &out_buf);
	    } else {
		rc = strcpy(out_buf, help);
	    }

	    rc = write(connfd, out_buf, strlen(out_buf));
	    check(rc != -1, "Failed to write to socket.");
	    memset(in_buf, 0, 1025);
	    memset(out_buf, 0, 1025);
	}

	bdestroy(input);
	bstrListDestroy(data);
	close(connfd);
    }

    close(socket);
    free(in_buf);
    free(out_buf);

    return 0;

error:
    if (in_buf) free(in_buf);
    if (out_buf) free(out_buf);
    if (input) bdestroy(input);
    if (connfd > 0) close(connfd);
    if (socket > 0) close(socket);
    return -1;
}
