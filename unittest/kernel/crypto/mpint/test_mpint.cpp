/**
 * @file test_mpint.cpp
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-05-09
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

#include <gmock/gmock.h>
#include <gtest/gtest.h>

/*
class MockCrypto
{
public:
    MOCK_METHOD(void *, crypto_zeroize, (void *, size_t));
    MOCK_METHOD(void, crypto_free, (void *));
};

MockCrypto _mock_crypto;

extern "C"
{
    void *crypto_zeroize(void *p, size_t s)
    {
        return _mock_crypto.crypto_zeroize(p, s);
    }

    void crypto_free(void *p)
    {
        return _mock_crypto.crypto_free(p);
    }
}
*/

extern "C"
{
#include "../../../../kernel/src/crypto/crypto_free.c"
#include "../../../../kernel/src/crypto/crypto_malloc.c"
#include "../../../../kernel/src/crypto/crypto_util.c"
#include "../../../../kernel/src/crypto/crypto_zeroize.c"
#include "../../../../kernel/src/crypto/mpint/mpint_add.c"
#include "../../../../kernel/src/crypto/mpint/mpint_add_abs.c"
#include "../../../../kernel/src/crypto/mpint/mpint_add_int.c"
#include "../../../../kernel/src/crypto/mpint/mpint_add_mod.c"
#include "../../../../kernel/src/crypto/mpint/mpint_comp.c"
#include "../../../../kernel/src/crypto/mpint/mpint_comp_abs.c"
#include "../../../../kernel/src/crypto/mpint/mpint_comp_int.c"
#include "../../../../kernel/src/crypto/mpint/mpint_copy.c"
#include "../../../../kernel/src/crypto/mpint/mpint_div.c"
#include "../../../../kernel/src/crypto/mpint/mpint_div_int.c"
#include "../../../../kernel/src/crypto/mpint/mpint_exp_mod.c"
#include "../../../../kernel/src/crypto/mpint/mpint_free.c"
#include "../../../../kernel/src/crypto/mpint/mpint_get_bit_length.c"
#include "../../../../kernel/src/crypto/mpint/mpint_get_bit_value.c"
#include "../../../../kernel/src/crypto/mpint/mpint_get_byte_length.c"
#include "../../../../kernel/src/crypto/mpint/mpint_get_length.c"
#include "../../../../kernel/src/crypto/mpint/mpint_grow.c"
#include "../../../../kernel/src/crypto/mpint/mpint_init.c"
#include "../../../../kernel/src/crypto/mpint/mpint_inv_mod.c"
#include "../../../../kernel/src/crypto/mpint/mpint_mod.c"
#include "../../../../kernel/src/crypto/mpint/mpint_montgomery_mul.c"
#include "../../../../kernel/src/crypto/mpint/mpint_montgomery_red.c"
#include "../../../../kernel/src/crypto/mpint/mpint_mul.c"
#include "../../../../kernel/src/crypto/mpint/mpint_mul_acc_core.c"
#include "../../../../kernel/src/crypto/mpint/mpint_mul_int.c"
#include "../../../../kernel/src/crypto/mpint/mpint_mul_mod.c"
#include "../../../../kernel/src/crypto/mpint/mpint_set_bit_value.c"
#include "../../../../kernel/src/crypto/mpint/mpint_set_value.c"
#include "../../../../kernel/src/crypto/mpint/mpint_shift_left.c"
#include "../../../../kernel/src/crypto/mpint/mpint_shift_right.c"
#include "../../../../kernel/src/crypto/mpint/mpint_sub.c"
#include "../../../../kernel/src/crypto/mpint/mpint_sub_abs.c"
#include "../../../../kernel/src/crypto/mpint/mpint_sub_int.c"
#include "../../../../kernel/src/crypto/mpint/mpint_sub_mod.c"
#include "../../../../kernel/src/crypto/mpint/mpint_swap.c"
}

//==============================================================================
// Test fixtures
//==============================================================================

class MpintTest : public ::testing::Test
{
public:
    void SetUp() override
    {
        mpint_init(&a);
        mpint_init(&b);
    }

    void TearDown() override
    {
        mpint_free(&a);
        mpint_free(&b);
    }

    mpint_t a;
    mpint_t b;
};

//==============================================================================
// Tests
//==============================================================================

TEST_F(MpintTest, SetUp)
{
}

//==============================================================================
// Main file
//==============================================================================

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

//==============================================================================
// End of file
//==============================================================================
