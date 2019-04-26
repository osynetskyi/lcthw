#include <lcthw/tstree.h>
#include <lcthw/bstrlib.h>
#include <dlfcn.h>
//#include <lcthw/dbg.h>

typedef struct Handler {
    bstring name;
    bstring url;
    bstring function;
} Handler;

typedef int (*lib_function) (const char *data);

TSTree *add_route_data(TSTree * routes, bstring line)
{
    struct bstrList *data = bsplit(line, ' ');
    check(data != NULL, "Bad data from bsplit.");
    check(data->qty > 1, "Line '%s' has less than 2 columns",
	    bdata(line));

    Handler *handler = malloc(sizeof(Handler));
    handler->url = bstrcpy(data->entry[0]);
    handler->name = bstrcpy(data->entry[1]);
    if (data->qty == 3) {
        handler->function = bstrcpy(data->entry[2]);
    } else {
        handler->function = NULL;
    }

    /*routes = TSTree_insert(routes,
	    bdata(data->entry[0]),
	    blength(data->entry[0]),
	    bstrcpy(data->entry[1]));*/

    routes = TSTree_insert(routes,
	    bdata(handler->url),
	    blength(handler->url),
	    handler);

    bstrListDestroy(data);

    return routes;

error:
    if (data) bstrListDestroy(data);
    return NULL;
}

TSTree *load_routes(const char *file)
{
    TSTree *routes = NULL;
    bstring line = NULL;
    FILE *routes_map = NULL;

    routes_map = fopen(file, "r");
    check(routes_map != NULL, "Failed to open routes: %s", file);

    while ((line = bgets((bNgetc) fgetc, routes_map, '\n')) != NULL) {
        check(btrimws(line) == BSTR_OK, "Failed to trim line.");
	routes = add_route_data(routes, line);
	check(routes != NULL, "Failed to add route.");
	bdestroy(line);
	line = NULL;
    }

    fclose(routes_map);
    return routes;

error:
    if (routes_map) fclose(routes_map);
    if (line) bdestroy(line);

    return NULL;
}

bstring match_url(TSTree * routes, bstring url, char **method)
{
    //bstring route = TSTree_search(routes, bdata(url), blength(url));
    Handler *handler = TSTree_search(routes, bdata(url), blength(url));
    bstring route = NULL;

    if (handler) {
        route = handler->name;
	if (handler->function)
	    *method = bdata(handler->function);
    } else  {
        printf("No exact match found, trying prefix.\n");
	//route = TSTree_search_prefix(routes, bdata(url), blength(url));

	DArray *tmp = TSTree_collect(routes, bdata(url), blength(url));
	if (tmp && DArray_count(tmp) > 0) {
	    int i = 0;
	    char *max_key = NULL;
	    int max = 0;
	    for (i = 0; i < DArray_count(tmp); i++) {
		bstring cur = (bstring)DArray_get(tmp, i);
	        //printf("%s\n", bdata(cur));
		if (blength(cur) > max) {
		    max = blength(cur);
		    max_key = bdata(cur);
		}
	    }

	    //route = TSTree_search(routes, max_key, max);
	    Handler *handler = TSTree_search(routes, max_key, max);
	    route = handler->name;
	    
	    for (i = 0; i < DArray_count(tmp); i++) {
	        bdestroy((bstring)DArray_get(tmp, i));
	    }
	    DArray_destroy(tmp);
	}
    }

    return route;
}

bstring read_line(const char *prompt)
{
    printf("%s", prompt);

    bstring result = bgets((bNgetc) fgetc, stdin, '\n');
    check_debug(result != NULL, "stdin closed.");

    check(btrimws(result) == BSTR_OK, "Failed to trim.");

    return result;

error:
    if (result) bdestroy(result);
    return NULL;
}

void bdestroy_cb(void *value, void *ignored)
{
    (void)ignored;
    //bdestroy((bstring) value);
    Handler *handler = (Handler *)value;
    bdestroy(handler->url);
    bdestroy(handler->name);
    if (handler->function) bdestroy(handler->function);
    free(handler);
}

void destroy_routes(TSTree * routes)
{
    TSTree_traverse(routes, bdestroy_cb, NULL);
    TSTree_destroy(routes);
}

int main(int argc, char *argv[])
{
    bstring url = NULL;
    bstring route = NULL;
    TSTree *routes = NULL;

    check(argc == 2, "USAGE: urlor <urlfile>");

    routes = load_routes(argv[1]);
    check(routes != NULL, "Your route file has an error.");

    while (1) {
        url = read_line("URL> ");
	check_debug(url != NULL, "goodbye.");
	if (blength(url) == 0) {
	    bdestroy(url);
	    break;
	}

	char *method = NULL;

	route = match_url(routes, url, &method);

	if (route) {
	    printf("MATCH: %s == %s\n", bdata(url), bdata(route));
	    if (method) {
		char *lib_name = bdata(route);
	        void *lib = dlopen(lib_name, RTLD_NOW);
		check(lib, "Failed to open library %s: %s", lib_name,
			dlerror());

		lib_function func = dlsym(lib, method);
		check(func, "Did not find %s function in the library %s: %s",
			method, lib_name, dlerror());

		int rc = func("test string");
		check(rc == 0, "Function %s returned %d", method, rc);

		rc = dlclose(lib);
		check(rc == 0, "Failed to close %s", lib_name);
	    }
	} else {
	    printf("FAIL: %s\n", bdata(url));
	}

	bdestroy(url);
    }

    destroy_routes(routes);
    return 0;

error:
    if (routes) destroy_routes(routes);
    return 1;
}
