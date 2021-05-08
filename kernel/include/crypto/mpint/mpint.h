/**
 * @file mpint.h
 * @author Joakim Bertils
 * @version 0.1
 * @date 2021-05-08
 *
 * @brief Representation of arbitrarily large integers.
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

#ifndef _CRYPTO_MPINT_H
#define _CRYPTO_MPINT_H

#include <crypto/crypto.h>
#include <logging/logging.h>

#include <stdint.h>

/**
 * @brief Goto the specified label if expr returns non-zero.
 *
 *
 */
#define MPINT_GOTO_ON_ERROR(expr, label)                     \
    error = (expr);                                          \
    if (error != 0)                                          \
    {                                                        \
        log_error("Error evaluating expression: %s", #expr); \
        goto label;                                          \
    }

#define MPINT_NODISCARD __attribute__((warn_unused_result))

typedef struct
{
    int sign;
    size_t size;
    uint32_t *data;

} mpint_t;

/**
 * @brief The size of the underlying type
 *
 *
 */
#define MPINT_INT_SIZE sizeof(*((mpint_t *)0)->data)

/**
 * @brief Initialize the integer data type.
 *
 * Must be called before using the integer in any way.
 *
 * @param i Pointer to where the integer is stored.
 *
 */
void mpint_init(mpint_t *i);

/**
 * @brief Frees and clears the memory associated with the integer.
 *
 * @param i Pointer to where the integer is stored.
 *
 */
void mpint_free(mpint_t *i);

/**
 * @brief Grows the integer, making room for @a new_size number of words.
 *
 * @param i Pointer to where the integer is stored.
 * @param new_size New requested size of the integer.
 *
 * @return Non-zero on allocation error.
 */
int mpint_grow(mpint_t *i, size_t new_size) MPINT_NODISCARD;

/**
 * @brief Get the length of the integer in words.
 *
 * @param a Pointer to where the integer is stored.
 *
 * @return Size of the integer in words.
 */
size_t mpint_get_length(const mpint_t *a);

/**
 * @brief Get the length of the integer in bytes.
 *
 * @param a Pointer to where the integer is stored.
 *
 * @return Size of the integer in words.
 */
size_t mpint_get_byte_length(const mpint_t *a);

/**
 * @brief Get the length of the integer in bits.
 *
 * @param a Pointer to where the integer is stored.
 *
 * @return Size of the integer in bits.
 */
size_t mpint_get_bit_length(const mpint_t *a);

/**
 * @brief Sets the bit value at the specified index.
 *
 * @param i Pointer to where the integer is stored.
 * @param index Index of the bit to be written.
 * @param value Value to write to the bit.
 *
 * @return Non-zero on allocation error.
 */
int mpint_set_bit_value(mpint_t *i,
                        size_t index,
                        uint32_t value) MPINT_NODISCARD;

/**
 * @brief Gets the bit value at the specified index.
 *
 * @param i Pointer to where the integer is stored.
 * @param index Index of the bit to be written.
 *
 * @return Value of the bit at index @a index.
 */
uint32_t mpint_get_bit_value(const mpint_t *i, size_t index);

/**
 * @brief Compares two integers.
 *
 * @param a Pointer to where the first integer is stored.
 * @param b Pointer to where the second integer is stored.
 *
 * @return >0 if a > b, <0 if a < b, else 0.
 */
int mpint_comp(const mpint_t *a, const mpint_t *b);

/**
 * @brief Compares the absolute value of two integers.
 *
 * @param a Pointer to where the first integer is stored.
 * @param b Pointer to where the second integer is stored.
 *
 * @return >0 if |a| > |b|, <0 if |a| < |b|, else 0.
 */
int mpint_comp_abs(const mpint_t *a, const mpint_t *b);

/**
 * @brief Compares an mpint with a primitive integer.
 *
 * @param a Pointer to where the first integer is stored.
 * @param b Primitive integer.
 *
 * @return >0 if a > b, <0 if a < b, else 0.
 */
int mpint_comp_int(const mpint_t *a, int b);

/**
 * @brief Copies the value of an integer to another integer.
 *
 * @param dest Pointer to the destination integer.
 * @param src Pointer to the source integer.
 *
 * @return Non-zero on allocation error.
 */
int mpint_copy(mpint_t *dest, const mpint_t *src) MPINT_NODISCARD;

/**
 * @brief Sets the value of the integer.
 *
 * @param a Pointer to the integer to set.
 * @param b Value to set.
 *
 * @return Non-zero on allocation error.
 */
int mpint_set_value(mpint_t *a, int b) MPINT_NODISCARD;

/**
 * @brief Adds two integers.
 *
 * @param r Pointer to destination.
 * @param a Pointer to first operand.
 * @param b Pointer to second operand.
 *
 * @return Non-zero on allocation error.
 */
int mpint_add(mpint_t *r, const mpint_t *a, const mpint_t *b) MPINT_NODISCARD;

/**
 * @brief Adds an mpint with primitive int.
 *
 * @param r Pointer to destination.
 * @param a Pointer to first operand.
 * @param b Value to add.
 *
 * @return Non-zero on allocation error.
 */
int mpint_add_int(mpint_t *r, const mpint_t *a, int b) MPINT_NODISCARD;

/**
 * @brief Subtracts two integers.
 *
 * @param r Pointer to destination.
 * @param a Pointer to first operand.
 * @param b Pointer to second operand.
 *
 * @return Non-zero on allocation error.
 */
int mpint_sub(mpint_t *r, const mpint_t *a, const mpint_t *b) MPINT_NODISCARD;

/**
 * @brief Subtracts a primitive int from a mpint.
 *
 * @param r Pointer to destination.
 * @param a Pointer to first operand.
 * @param b Value to subtract.
 *
 * @return Non-zero on allocation error.
 */
int mpint_sub_int(mpint_t *r, const mpint_t *a, int b) MPINT_NODISCARD;

/**
 * @brief Adds two integers and calculate the absolute value of the sum.
 *
 * @param r Pointer to destination.
 * @param a Pointer to first operand.
 * @param b Pointer to second operand.
 *
 * @return Non-zero on allocation error.
 */
int mpint_add_abs(mpint_t *r,
                  const mpint_t *a,
                  const mpint_t *b) MPINT_NODISCARD;

/**
 * @brief Subtracts two integers and calculate the absolute value of the
 * difference.
 *
 * @param r Pointer to destination.
 * @param a Pointer to first operand.
 * @param b Pointer to second operand.
 *
 * @return Non-zero on allocation error.
 */
int mpint_sub_abs(mpint_t *r,
                  const mpint_t *a,
                  const mpint_t *b) MPINT_NODISCARD;

/**
 * @brief Shifts the integer @a n bits to the left.
 *
 * @param r Pointer to integer.
 * @param n Number of bits to shift.
 *
 * @return Non-zero on error.
 */
int mpint_shift_left(mpint_t *r, size_t n) MPINT_NODISCARD;

/**
 * @brief Shifts the integer @a n bits to the right.
 *
 * @param r Pointer to integer.
 * @param n Number of bits to shift.
 *
 * @return Non-zero on error.
 */
int mpint_shift_right(mpint_t *r, size_t n) MPINT_NODISCARD;

/**
 * @brief Mutliplies two integers.
 *
 * @param r Pointer to destination.
 * @param a Pointer to first operand.
 * @param b Pointer to second operand.
 *
 * @return Non-zero on allocation error.
 */
int mpint_mul(mpint_t *r, const mpint_t *a, const mpint_t *b) MPINT_NODISCARD;

/**
 * @brief Multiplies an mpint with primitive int.
 *
 * @param r Pointer to destination.
 * @param a Pointer to first operand.
 * @param b Value to multiply with.
 *
 * @return Non-zero on allocation error.
 */
int mpint_mul_int(mpint_t *r, const mpint_t *a, int b) MPINT_NODISCARD;

/**
 * @brief Divides two integers.
 *
 * @param r Pointer to destination.
 * @param a Pointer to first operand.
 * @param b Pointer to second operand.
 *
 * @return Non-zero on allocation error or divide by zero.
 */
int mpint_div(mpint_t *q, mpint_t *r, const mpint_t *a, const mpint_t *b)
    MPINT_NODISCARD;

/**
 * @brief Adds an mpint with primitive int.
 *
 * @param r Pointer to destination.
 * @param a Pointer to first operand.
 * @param b Value to add.
 *
 * @return Non-zero on allocation error.
 */
int mpint_div_int(mpint_t *q, mpint_t *r, const mpint_t *a, int b)
    MPINT_NODISCARD;

/**
 * @brief Find the inverse of A mod P
 *
 * @param r Pointer to destination.
 * @param a Pointer to operand.
 * @param p Pointer to modulus.
 *
 * @return Non-zero on allocation error.
 */
int mpint_mod(mpint_t *r, const mpint_t *a, const mpint_t *p) MPINT_NODISCARD;

/**
 * @brief Adds two integers under modulus.
 *
 * @param r Pointer to destination.
 * @param a Pointer to first operand.
 * @param b Pointer to second operand.
 * @param p Pointer to modulus.
 *
 * @return Non-zero on allocation error.
 */
int mpint_add_mod(mpint_t *r,
                  const mpint_t *a,
                  const mpint_t *b,
                  const mpint_t *p) MPINT_NODISCARD;

/**
 * @brief Subtracts two integers under modulus.
 *
 * @param r Pointer to destination.
 * @param a Pointer to first operand.
 * @param b Pointer to second operand.
 * @param p Pointer to modulus.
 *
 * @return Non-zero on allocation error.
 */
int mpint_sub_mod(mpint_t *r,
                  const mpint_t *a,
                  const mpint_t *b,
                  const mpint_t *p) MPINT_NODISCARD;

/**
 * @brief Multiplies two integers under modulus.
 *
 * @param r Pointer to destination.
 * @param a Pointer to first operand.
 * @param b Pointer to second operand.
 * @param p Pointer to modulus.
 *
 * @return Non-zero on allocation error.
 */
int mpint_mul_mod(mpint_t *r,
                  const mpint_t *a,
                  const mpint_t *b,
                  const mpint_t *p) MPINT_NODISCARD;

/**
 * @brief Find inverse of @a a under modulus @a p.
 *
 * Finds R such that R * A mod P = 1 mod P
 *
 * @param r Pointer to destination.
 * @param a Pointer to first operand.
 * @param p Pointer to modulus.
 *
 * @return Non-zero on allocation error.
 */
int mpint_inv_mod(mpint_t *r,
                  const mpint_t *a,
                  const mpint_t *p) MPINT_NODISCARD;

/**
 * @brief Modular exponentiation.
 *
 * R = B ^ E mod P
 *
 * @param r Pointer to destination
 * @param b Pointer to base operand.
 * @param e Pointer to exponent operand.
 * @param p Pointer to modulus operand.
 *
 * @return Non-zero on allocation error.
 */
int mpint_exp_mod(mpint_t *r,
                  const mpint_t *b,
                  const mpint_t *e,
                  const mpint_t *p) MPINT_NODISCARD;

/**
 * @brief Calculate the Montgomery modular multiplication
 *
 * given by the formula
 *
 * R = A * B / 2^k mod P
 *
 * with the following conditions:
 *
 * 0 <= A < 2^k,
 * 0 <= B < 2^k
 *
 * and
 *
 * P < 2^k
 *
 * @param r Pointer to the resulting integer.
 * @param a Pointer to the A operand.
 * @param b Pointer to the B operand.
 * @param k Value of k.
 * @param p Pointer to modulus P.
 * @param t Pointer to integer used as workspace.
 *
 * @return Non-zero on error.
 */
int mpint_montgomery_mul(mpint_t *r,
                         const mpint_t *a,
                         const mpint_t *b,
                         uint32_t k,
                         const mpint_t *p,
                         mpint_t *t) MPINT_NODISCARD;

/**
 * @brief Calculate the Montgomery reduction.
 *
 * The reduction is given by the formula
 *
 * R = A / 2^k mod P
 *
 * with the following conditions:
 *
 * 0 <= A < 2^k
 *
 * and
 *
 * P < 2^k
 *
 * @param r Pointer to destination.
 * @param a Pointer to A operand.
 * @param k Value of k.
 * @param p Pointer to modulus P.
 * @param t Pointer to integer used as workspace.
 *
 * @return Non-zero on allocation error.
 */
int mpint_montgomery_red(mpint_t *r,
                         const mpint_t *a,
                         uint32_t k,
                         const mpint_t *p,
                         mpint_t *t) MPINT_NODISCARD;

/**
 * @brief Core kernel of the multiply accumulate operation.
 *
 * @param r Pointer to resulting integer.
 * @param a First operand.
 * @param m Size of the first operand @a a.
 * @param b Second operand.
 *
 */
void mpint_mul_acc_core(uint32_t *r,
                        const uint32_t *a,
                        size_t m,
                        const uint32_t b);

/**
 * @brief Check if the number is a probable prime, using the Miller-Rabin test.
 *
 * @param a Pointer to where the integer is stored.
 *
 * @return Non-zero if probable prime.
 */
int mpint_check_probable_prime(const mpint_t *a);

/**
 * @brief Swaps two integers.
 *
 * @param a Pointer to first integer.
 * @param b Pointer to second integer.
 *
 */
void mpint_swap(const mpint_t *a, const mpint_t *b);

#endif

//=============================================================================
// End of file
//=============================================================================
