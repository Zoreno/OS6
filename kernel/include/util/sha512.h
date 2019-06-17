#ifndef _SHA512_H
#define _SHA512_H

#include <stdarg.h>
#include <stdint.h>

#define SHA512_DIGEST_SIZE 64

void sha512(const uint8_t *in, size_t n, uint8_t *out);

#endif