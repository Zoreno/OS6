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
#include "../../../../kernel/src/crypto/mpint/mpint.c"
}

//==============================================================================
// Test fixtures
//==============================================================================

using uvec32 = std::vector<uint32_t>;

class MpintTest : public ::testing::Test
{
public:
    void SetUp() override
    {
        mpint_init(&a);
        mpint_init(&b);
        mpint_init(&c);
        mpint_init(&r);
    }

    void TearDown() override
    {
        mpint_free(&a);
        mpint_free(&b);
        mpint_free(&c);
        mpint_free(&r);
    }

    mpint_t a;
    mpint_t b;
    mpint_t c;
    mpint_t r;

    void SetValue(mpint_t *a, int value)
    {
        EXPECT_EQ(mpint_set_value(a, value), 0);
    }

    void SetValue(mpint_t *a, const uvec32 &data, int sign = 1)
    {
        Grow(a, data.size());
        a->sign = sign;

        for (size_t i = 0; i < data.size(); ++i)
        {
            a->data[i] = data.at(i);
        }
    }

    void Grow(mpint_t *a, size_t new_size)
    {
        EXPECT_EQ(mpint_grow(a, new_size), 0);
    }

    void Add(mpint_t *r, mpint_t *a, mpint_t *b)
    {
        EXPECT_EQ(mpint_add(r, a, b), 0);
    }

    void AddInt(mpint_t *r, mpint_t *a, int b)
    {
        EXPECT_EQ(mpint_add_int(r, a, b), 0);
    }

    void Sub(mpint_t *r, mpint_t *a, mpint_t *b)
    {
        EXPECT_EQ(mpint_sub(r, a, b), 0);
    }

    void SubInt(mpint_t *r, mpint_t *a, int b)
    {
        EXPECT_EQ(mpint_sub_int(r, a, b), 0);
    }

    void Mul(mpint_t *r, mpint_t *a, mpint_t *b)
    {
        EXPECT_EQ(mpint_mul(r, a, b), 0);
    }

    void MulInt(mpint_t *r, mpint_t *a, int b)
    {
        EXPECT_EQ(mpint_mul_int(r, a, b), 0);
    }

    void Div(mpint_t *q, mpint_t *r, mpint_t *a, mpint_t *b)
    {
        EXPECT_EQ(mpint_div(q, r, a, b), 0);
    }

    void DivInt(mpint_t *q, mpint_t *r, mpint_t *a, int b)
    {
        EXPECT_EQ(mpint_div_int(q, r, a, b), 0);
    }

    void ExpMod(mpint_t *r, mpint_t *b, mpint_t *e, mpint_t *p)
    {
        EXPECT_EQ(mpint_exp_mod(r, b, e, p), 0);
    }

    void MulMod(mpint_t *r,
                const mpint_t *a,
                const mpint_t *b,
                const mpint_t *p)
    {
        EXPECT_EQ(mpint_mul_mod(r, a, b, p), 0);
    }

    void Mod(mpint_t *r, const mpint_t *a, const mpint_t *p)
    {
        EXPECT_EQ(mpint_mod(r, a, p), 0);
    }

    void ExpectValue(mpint_t *a, const uvec32 &data, int sign = 0)
    {
        size_t i;

        size_t max_size = std::min(a->size, data.size());

        for (i = 0; i < max_size; ++i)
        {
            EXPECT_EQ(a->data[i], data.at(i)) << "Index: " << i;
        }

        for (; i < a->size; ++i)
        {
            EXPECT_EQ(a->data[i], 0) << "Index: " << i;
        }

        if (sign != 0)
        {
            EXPECT_EQ(a->sign, sign);
        }
    }

    void ExpectSize(mpint_t *a, size_t size)
    {
        EXPECT_EQ(a->size, size);
    }
};

//==============================================================================
// Tests
//==============================================================================

TEST_F(MpintTest, SetUp)
{
}

TEST_F(MpintTest, Grow)
{
    Grow(&a, 1);
    ExpectSize(&a, 1);
    Grow(&a, 2);
    ExpectSize(&a, 2);
    Grow(&a, 1);
    ExpectSize(&a, 2);
    Grow(&a, 0);
    ExpectSize(&a, 2);
}

TEST_F(MpintTest, GetLength)
{
    EXPECT_EQ(mpint_get_length(&a), 0);

    SetValue(&a, {0x1234, 0x1234});

    EXPECT_EQ(mpint_get_length(&a), 2);
}

TEST_F(MpintTest, GetByteLength)
{
    EXPECT_EQ(mpint_get_byte_length(&a), 0);

    SetValue(&a, {0x12347263, 0x1234});

    EXPECT_EQ(mpint_get_byte_length(&a), 6);
}

TEST_F(MpintTest, GetBitLength)
{
    EXPECT_EQ(mpint_get_bit_length(&a), 0);

    SetValue(&a, {0x12347263, 0x1234});

    EXPECT_EQ(mpint_get_bit_length(&a), 45);
}

TEST_F(MpintTest, Add)
{
    SetValue(&a, {0x1});
    SetValue(&b, {0x2});
    Add(&r, &a, &b);
    ExpectValue(&r, {0x3});

    SetValue(&a, {0x1});
    SetValue(&b, {0x2}, -1);
    Add(&r, &a, &b);
    ExpectValue(&r, {0x1}, -1);

    SetValue(&a, uvec32{0x80000000U});
    SetValue(&b, uvec32{0x80000000U});
    Add(&r, &a, &b);
    ExpectValue(&r, uvec32{0x0, 0x1});

    SetValue(&a, uvec32{0x80000000U}, -1);
    SetValue(&b, uvec32{0x80000000U});
    Add(&r, &a, &b);
    ExpectValue(&r, uvec32{0x0, 0x0}, -1);
}

TEST_F(MpintTest, AddInt)
{
    SetValue(&a, {0x1});
    AddInt(&r, &a, 0x8);
    ExpectValue(&r, {0x9});
}

TEST_F(MpintTest, Sub)
{
    SetValue(&a, {0x1});
    SetValue(&b, {0x1});
    Sub(&r, &a, &b);
    ExpectValue(&r, {0x0});
}

TEST_F(MpintTest, SubInt)
{
    SetValue(&a, {0x9});

    SubInt(&r, &a, 8);
    ExpectValue(&r, {0x1});
}

TEST_F(MpintTest, Mul)
{
    SetValue(&a, {0x41});
    SetValue(&b, {0x52});
    Mul(&r, &a, &b);
    ExpectValue(&r, {0x14d2});

    SetValue(&a, {0x9832});
    SetValue(&b, {0x8239});
    Mul(&r, &a, &b);
    ExpectValue(&r, {0x4d6b4722});

    SetValue(&a, uvec32{0x98329833U});
    SetValue(&b, uvec32{0x82398393U});
    Mul(&r, &a, &b);
    ExpectValue(&r, uvec32{0x124a7e49, 0x4d6be2c7});
}

TEST_F(MpintTest, MulInt)
{
    SetValue(&a, {0x41});
    MulInt(&r, &a, 0x52);
    ExpectValue(&r, {0x14d2});
}

TEST_F(MpintTest, MulMod)
{
    SetValue(&a, {20});
    SetValue(&b, {82});
    MulMod(&r, &a, &a, &b);
    ExpectValue(&r, {72});
}

TEST_F(MpintTest, Mod)
{
    SetValue(&a, {2});
    SetValue(&b, {3});
    Mod(&r, &a, &b);
    ExpectValue(&r, {2});
}

TEST_F(MpintTest, Mod2)
{
    SetValue(&a, {17});
    SetValue(&b, {12});
    Mod(&r, &a, &b);
    ExpectValue(&r, {5});
}

TEST_F(MpintTest, ExpMod1)
{
    SetValue(&a, {2});  // Base
    SetValue(&b, {3});  // Exponent
    SetValue(&c, {7});  // Modulus
    ExpMod(&r, &a, &b, &c);
    ExpectValue(&r, {1});
}

TEST_F(MpintTest, ExpMod2)
{
    SetValue(&a, {102});  // Base
    SetValue(&b, {103});  // Exponent
    SetValue(&c, {82});   // Modulus
    ExpMod(&r, &a, &b, &c);
    ExpectValue(&r, {46});
}

TEST_F(MpintTest, ExpMod3)
{
    SetValue(&a, {102});  // Base
    SetValue(&b, {103});  // Exponent
    SetValue(&c, {81});   // Modulus
    ExpMod(&r, &a, &b, &c);
    ExpectValue(&r, {0});
}

TEST_F(MpintTest, ExpMod4)
{
    SetValue(&a, {98237});  // Base
    SetValue(&b, {98233});  // Exponent
    SetValue(&c, {83923});  // Modulus
    ExpMod(&r, &a, &b, &c);
    ExpectValue(&r, {26949});
}

TEST_F(MpintTest, ExpMod5)
{
    SetValue(&a, uvec32{0x78398237U, 0x78398237U});  // Base
    SetValue(&b, uvec32{0x83398233U, 0x83398233U});  // Exponent
    SetValue(&c, uvec32{0x38783923U, 0x38783923U});  // Modulus
    ExpMod(&r, &a, &b, &c);
    ExpectValue(&r, uvec32{0x136206f4U, 0x136206f4U});
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
