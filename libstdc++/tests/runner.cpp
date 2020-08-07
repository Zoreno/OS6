#include <__test/__test.hpp>

#include <stdio.h>

TEST_GROUP(dummy){

};

TEST(dummy, dummy)
{
    return true;
}

TEST_GROUP(testing){
    public :
        void setup() override{
            //printf("This is in setup\n");
        }

    void teardown() override{
        //printf("This is in teardown\n");
    }
};

TEST(testing, static_init)
{
    return true;
}

TEST(testing, static_init2)
{
    return true;
}

void run_tests(void)
{
    printf("Running tests\n");

    __test *dummy = &dummy_dummy_testimpl;
    __test *first = dummy;
    __test *last = dummy;

    while (true)
    {
        if (first->magic != 0xDEADBEEF)
        {
            ++first;
            break;
        }

        --first;
    }

    while (true)
    {
        if (last->magic != 0xDEADBEEF)
        {
            break;
        }

        ++last;
    }

    int count = (last - first) - 1;
    int failed = 0;

    printf("[==========] Running %i tests\n", count);

    for (__test *curr = first; curr != last; ++curr)
    {
        if (curr != dummy)
        {
            printf("[ RUN      ] %s\n", curr->get_test_name());

            curr->do_setup();

            bool result = curr->do_testbody();

            curr->do_teardown();

            if (result)
            {
                printf("[       OK ] %s\n", curr->get_test_name());
            }
            else
            {
                ++failed;
                printf("[  FAILED  ] %s\n", curr->get_test_name());
            }
        }
    }

    if (failed)
    {
        printf("[==========] %i test(s) failed\n", failed);
    }
    else
    {
        printf("[==========] All tests passed!\n");
    }
}