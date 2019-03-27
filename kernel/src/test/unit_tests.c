
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

static int num_tests = 0;
static int num_cleared = 0;

#define TEST_INT(expr, expected)                        \
    {                                                   \
        int eval = (expr);                              \
        if (eval == (expected))                         \
        {                                               \
            ++num_cleared;                              \
        }                                               \
        else                                            \
        {                                               \
            printf("ERROR: [%s] got: %i expected %i\n", \
                   #expr, eval, expected);              \
        }                                               \
                                                        \
        ++num_tests;                                    \
    }

#define TEST_CHAR(expr, expected)                       \
    {                                                   \
        char eval = (expr);                             \
        if (eval == (expected))                         \
        {                                               \
            ++num_cleared;                              \
        }                                               \
        else                                            \
        {                                               \
            printf("ERROR: [%s] got: %c expected %c\n", \
                   #expr, eval, expected);              \
        }                                               \
                                                        \
        ++num_tests;                                    \
    }

#define TEST_STRING(expr, expected)                             \
    {                                                           \
        char *eval = (expr);                                    \
        if (strcmp(eval, expected) == 0)                        \
        {                                                       \
            ++num_cleared;                                      \
        }                                                       \
        else                                                    \
        {                                                       \
            printf("ERROR: [%s] got: \"%s\" expected \"%s\"\n", \
                   #expr, eval, expected);                      \
        }                                                       \
                                                                \
        ++num_tests;                                            \
    }

#define TEST_INT_ARRAY(expr, expected, count)            \
    {                                                    \
        int *eval = (expr);                              \
        int *exp = (expected);                           \
        if (memcmp(eval, exp, count * sizeof(int)) == 0) \
        {                                                \
            ++num_cleared;                               \
        }                                                \
        else                                             \
        {                                                \
            printf("ERROR: [%s] got:\n", #expr);         \
            for (int i = 0; i < count; ++i)              \
            {                                            \
                printf("%i ", eval[i]);                  \
            }                                            \
            printf("\nexpected: \n");                    \
            for (int i = 0; i < count; ++i)              \
            {                                            \
                printf("%i ", exp[i]);                   \
            }                                            \
            printf("\n");                                \
        }                                                \
                                                         \
        ++num_tests;                                     \
    }

void run_test_ctype()
{
    printf("Running ctype.h tests...\n");

    TEST_INT(isalnum('c'), 1);
    TEST_INT(isalpha('c'), 1);
    TEST_INT(iscntrl('c'), 0);
    TEST_INT(isgraph('c'), 1);
    TEST_INT(isdigit('c'), 0);
    TEST_INT(islower('c'), 1);
    TEST_INT(isprint('c'), 1);
    TEST_INT(ispunct('c'), 0);
    TEST_INT(isspace('c'), 0);
    TEST_INT(isupper('c'), 0);
    TEST_INT(isxdigit('c'), 1);

    TEST_CHAR(tolower('c'), 'c');
    TEST_CHAR(toupper('c'), 'C');

    TEST_CHAR(tolower('C'), 'c');
    TEST_CHAR(toupper('C'), 'C');

    TEST_INT(isascii('c'), 1);
    TEST_CHAR(toascii('c'), 'c');
}

void run_test_string()
{
    printf("Running string.h tests...\n");

    char *a = "Hello World!";
    char *b = "This is a longer string";

    TEST_INT(strlen(a), 12);
    TEST_INT(strnlen(b, 20), 20);

    char c[64];

    TEST_STRING(strcpy(c, a), "Hello World!");
    TEST_STRING(strcat(c, a), "Hello World!Hello World!");

    TEST_STRING(strchr(a, 'd'), "d!");
}

int cmp(const void *a, const void *b)
{
    int ai = *(int *)a;
    int bi = *(int *)b;

    return ai < bi;
}

void run_test_stdlib()
{
    printf("Running stdlib.h tests...\n");

    char c[64];

    itoa(32, 10, c);
    TEST_STRING(c, "32");

    itoa(-32, 10, c);
    TEST_STRING(c, "-32");

    itoa(0, 10, c);
    TEST_STRING(c, "0");

    itoa(35, 16, c);
    TEST_STRING(c, "23");

    itoa(30, 16, c);
    TEST_STRING(c, "1E");

    TEST_INT(atoi("30"), 30);
    TEST_INT(atoi("-30"), -30);
    TEST_INT(atoi("     30"), 30);

    TEST_INT(strtol("23", NULL, 16), 35);
    TEST_INT(strtoul("23", NULL, 16), 35);
    TEST_INT(strtol("-0x23", NULL, 16), -35);

    int values[10] = {23, 32, 10, 11, -1, 0, 24, 14, 9, 10};
    int correct[10] = {-1, 0, 9, 10, 10, 11, 14, 23, 24, 32};
    qsort(values, 10, sizeof(int), cmp);

    TEST_INT_ARRAY(values, correct, 10);

    TEST_INT(abs(-30), 30);
    TEST_INT(abs(30), 30);
}

void run_test_printf()
{
    printf("Running printf.h tests...\n");

    char buf[256] = {0};

    sprintf(buf, "Hello World!");
    TEST_STRING(buf, "Hello World!");

    sprintf(buf, "%i", 23);
    TEST_STRING(buf, "23");

    sprintf(buf, "%i, %i", 23, 24);
    TEST_STRING(buf, "23, 24");

    sprintf(buf, "String: %s", "Hello World!");
    TEST_STRING(buf, "String: Hello World!");

    sprintf(buf, "%l", 0x7FFFFFFFFFFFFFFF);
    TEST_STRING(buf, "9223372036854775807");

    sprintf(buf, "%#x", 23);
    TEST_STRING(buf, "0x17");

    sprintf(buf, "%+i", 23);
    TEST_STRING(buf, "+23");

    sprintf(buf, "%+i", 23);
    TEST_STRING(buf, "+23");

    sprintf(buf, "%4i", 23);
    TEST_STRING(buf, "  23");

    sprintf(buf, "%-4i", 23);
    TEST_STRING(buf, "23  ");

    sprintf(buf, "%04i", 23);
    TEST_STRING(buf, "0023");

    sprintf(buf, "%#06x", 23);
    TEST_STRING(buf, "0x000017");

    sprintf(buf, "%*i", 4, 23);
    TEST_STRING(buf, "  23");

    sprintf(buf, "%c", 'c');
    TEST_STRING(buf, "c");
}

void run_unit_tests()
{
    printf("\nStarting test suite...\n");

    run_test_ctype();
    run_test_string();
    run_test_stdlib();
    run_test_printf();

    printf("Tests cleared: %i/%i\n", num_cleared, num_tests);
}
