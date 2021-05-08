/**
 * @file test_crypto.c
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-05-08
 *
 * @brief
 *
 * @copyright Copyright (C) 2021,
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https: //www.gnu.org/licenses/>.
 *
 */

#include <crypto/crypto.h>
#include <crypto/mpint/mpint.h>
#include <logging/logging.h>

//=============================================================================
// Test framework
//=============================================================================

#define EXPECT_EQ(expr, value)                                \
    do                                                        \
    {                                                         \
        int exprVal = (expr);                                 \
        if (exprVal != value)                                 \
        {                                                     \
            log_error("Expression %s, Expected: %i, got: %i", \
                      #expr,                                  \
                      value,                                  \
                      exprVal);                               \
        }                                                     \
    } while (0)

#define EXPECT_NO_ERROR(expr)                                    \
    do                                                           \
    {                                                            \
        int exprVal = (expr);                                    \
        if (exprVal != 0)                                        \
        {                                                        \
            log_error("Expression %s, Got: %i", #expr, exprVal); \
        }                                                        \
    } while (0)

typedef struct
{
    mpint_t r;
    mpint_t a;
    mpint_t b;
    mpint_t p;
} test_mpint_context_t;

void test_setup_context(test_mpint_context_t *context)
{
    mpint_init(&context->r);
    mpint_init(&context->a);
    mpint_init(&context->b);
    mpint_init(&context->p);
}

void test_teardown_context(test_mpint_context_t *context)
{
    mpint_free(&context->r);
    mpint_free(&context->a);
    mpint_free(&context->b);
    mpint_free(&context->p);
}

//=============================================================================
// Test cases
//=============================================================================

void test_mpint_get_length()
{
    test_mpint_context_t context;
    test_setup_context(&context);

#define TEST_MPINT_GET_LENGTH_TESTCASE(a_val, exp_val)   \
    EXPECT_NO_ERROR(mpint_set_value(&context.a, a_val)); \
    EXPECT_EQ(mpint_get_length(&context.a), exp_val)

    TEST_MPINT_GET_LENGTH_TESTCASE(23, 1);

    test_teardown_context(&context);
}

void test_mpint_sub_abs()
{
    test_mpint_context_t context;
    test_setup_context(&context);

#define TEST_MPINT_SUB_ABS_TESTCASE(a_val, b_val, exp_val)              \
    EXPECT_NO_ERROR(mpint_set_value(&context.a, a_val));                \
    EXPECT_NO_ERROR(mpint_set_value(&context.b, b_val));                \
    EXPECT_NO_ERROR(mpint_sub_abs(&context.r, &context.a, &context.b)); \
    EXPECT_EQ(context.r.data[0], exp_val);

    TEST_MPINT_SUB_ABS_TESTCASE(2, 73, 71);
    TEST_MPINT_SUB_ABS_TESTCASE(73, 2, 71);
    TEST_MPINT_SUB_ABS_TESTCASE(73, 0, 73);

    test_teardown_context(&context);
}

//=============================================================================
// Test runner
//=============================================================================

void test_crypto()
{
    log_info("Starting crypto test suite");

    test_mpint_get_length();
    test_mpint_sub_abs();

    log_info("Crypto test suite done!");
}

//=============================================================================
// End of file
//=============================================================================
