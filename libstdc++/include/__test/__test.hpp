#ifndef __TEST_HPP
#define __TEST_HPP

#define TEST_MAGIC 0xDEADBEEF

class __test
{
public:
    int magic = TEST_MAGIC;

    void do_setup()
    {
        setup();
    }

    void do_teardown()
    {
        teardown();
    }

    bool do_testbody()
    {
        return testbody();
    }

    virtual const char *get_test_name() = 0;

private:
    virtual bool testbody() = 0;

    virtual void setup()
    {
    }

    virtual void teardown()
    {
    }
};

#define __TEST_GROUP(name) \
    class name##_testgroup \
        : public __test

#define __TEST(groupname, name)                      \
    class groupname##_##name##_test                  \
        : public groupname##_testgroup               \
    {                                                \
    public:                                          \
        virtual bool testbody() override;            \
        virtual const char *get_test_name() override \
        {                                            \
            return #groupname "." #name;             \
        }                                            \
    };                                               \
    INST_TEST(groupname, name);                      \
    bool groupname##_##name##_test::testbody()

#define __INST_TEST(groupname, name)                        \
    groupname##_##name##_test groupname##_##name##_testimpl \
        __attribute__((used, section(".ostest"), aligned(1))) = {}

#define TEST_GROUP(name) __TEST_GROUP(name)
#define TEST(groupname, name) __TEST(groupname, name)
#define INST_TEST(groupname, name) __INST_TEST(groupname, name)

extern "C++"
{
    void run_tests(void);
}

#endif