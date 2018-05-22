#include "minunit.h"
#include <lcthw/bstrlib.h>

static char *test1 = "This string is a test.";
static char *test2 = "Hello world.";
static bstring bstr1 = NULL;
static bstring bstr2 = NULL;

char *test_create()
{
    bstr1 = bfromcstr(test1);
    mu_assert(bstr1, "Creation failed.");

    bstr2 = blk2bstr(test2, 12);
    mu_assert(bstr2, "Block creation failed.");

    return NULL;
}

char *test_bstrcpy()
{
    bstring bstr3 = bstrcpy(bstr1);
    mu_assert(bstr3, "Copy failed.");
    mu_assert(biseq(bstr1, bstr3) != -1, "Comparison error.");
    mu_assert(biseq(bstr1, bstr3), "Bstrings are not the same.");

    bdestroy(bstr3);

    return NULL;
}

char *test_bassign()
{
    int rc = bassign(bstr1, bstr2);
    mu_assert(rc != BSTR_ERR, "Error on assignment.");
    mu_assert(biseq(bstr1, bstr2), "Not equal after assignment.");

    rc = bassigncstr(bstr1, test2);
    mu_assert(rc != BSTR_ERR, "Error on C str assignment.");
    mu_assert(bstricmp(bstr1, bstr2) == 0, "Not equal after C str assignment.");
    
    rc = bassignblk(bstr1, test1, 22);
    mu_assert(rc != BSTR_ERR, "Error on C str buk assignment.");
    
    return NULL;
}

char *test_bconcat()
{
    int rc = bconcat(bstr1, bstr2);
    mu_assert(rc == BSTR_OK, "Error on concatenation.");

    rc = binstr(bstr1, 0, bstr2);
    mu_assert(rc != BSTR_ERR, "binstr didn't find anything.");

    return NULL;
}

char *test_replace_format()
{
    bstring needle = bfromcstr("world");
    bstring replace = bfromcstr("plane");

    int rc = bfindreplace(bstr2, needle, replace, 0);
    mu_assert(rc != BSTR_ERR, "Error when replacing.");
    mu_assert(binstr(bstr2, 0, replace) != BSTR_ERR, "Replaceement failed.");

    bstring format_test = bformat("Hello, %s", needle->data);
    mu_assert(format_test, "Format failed.");
    mu_assert(binstr(format_test, 0, needle), "Can't find inserted.");

    bdestroy(format_test);
    bdestroy(needle);
    bdestroy(replace);

    return NULL;
}

char *test_split()
{
    unsigned char split = ' ';
    struct bstrList *list = bsplit(bstr1, split);
    mu_assert(list, "Splitting error.");
    mu_assert(list->qty == 6, "Wrong count.");

    bstrListDestroy(list);

    return NULL;
}

char *test_misc()
{
    int len = blength(bstr2);
    mu_assert(len == 12, "Wrong length");
    mu_assert(strcmp(bdata(bstr2), "Hello plane.") == 0, "Wrong data.");

    char test = bchar(bstr2, 1);
    mu_assert(test == 'e', "Wrong char.");

    return NULL;
}

char *test_bdestroy()
{
    int rc = bdestroy(bstr1);
    mu_assert(rc != BSTR_ERR, "Bstr1 destroy failed.");

    rc = bdestroy(bstr2);
    mu_assert(rc != BSTR_ERR, "Bstr2 destroy failed.");

    return NULL;
}

char *all_tests()
{
    mu_suite_start();

    mu_run_test(test_create);
    mu_run_test(test_bstrcpy);
    mu_run_test(test_bassign);
    mu_run_test(test_bconcat);
    mu_run_test(test_replace_format);
    mu_run_test(test_split);
    mu_run_test(test_misc);

    mu_run_test(test_bdestroy);

    return NULL;
}

RUN_TESTS(all_tests);
