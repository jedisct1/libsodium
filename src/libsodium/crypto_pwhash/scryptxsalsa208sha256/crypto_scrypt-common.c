/*-
 * Copyright 2013 Alexander Peslyak
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <stdint.h>
#include <string.h>

#include "crypto_scrypt.h"

#define escrypt_kdf escrypt_kdf_nosse

#define BYTES2CHARS(bytes) \
	((((bytes) * 8) + 5) / 6)

#define HASH_SIZE 32 /* bytes */
#define HASH_LEN BYTES2CHARS(HASH_SIZE) /* base-64 chars */

static const char * const itoa64 =
	"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static uint8_t * encode64_uint32(uint8_t * dst, size_t dstlen,
    uint32_t src, uint32_t srcbits)
{
	uint32_t bit;

	for (bit = 0; bit < srcbits; bit += 6) {
		if (dstlen < 1)
			return NULL;
		*dst++ = itoa64[src & 0x3f];
		dstlen--;
		src >>= 6;
	}

	return dst;
}

static uint8_t * encode64(uint8_t * dst, size_t dstlen,
    const uint8_t * src, size_t srclen)
{
	size_t i;

	for (i = 0; i < srclen; ) {
		uint8_t * dnext;
		uint32_t value = 0, bits = 0;
		do {
			value |= (uint32_t)src[i++] << bits;
			bits += 8;
		} while (bits < 24 && i < srclen);
		dnext = encode64_uint32(dst, dstlen, value, bits);
		if (!dnext)
			return NULL;
		dstlen -= dnext - dst;
		dst = dnext;
	}

	return dst;
}

static int decode64_one(uint32_t * dst, uint8_t src)
{
	const char * ptr = strchr(itoa64, src);
	if (ptr) {
		*dst = ptr - itoa64;
		return 0;
	}
	*dst = 0;
	return -1;
}

static const uint8_t * decode64_uint32(uint32_t * dst, uint32_t dstbits,
    const uint8_t * src)
{
	uint32_t bit;
	uint32_t value;

	value = 0;
	for (bit = 0; bit < dstbits; bit += 6) {
		uint32_t one;
		if (decode64_one(&one, *src)) {
			*dst = 0;
			return NULL;
		}
		src++;
		value |= one << bit;
	}

	*dst = value;
	return src;
}

uint8_t *
escrypt_r(escrypt_local_t * local,
    const uint8_t * passwd, size_t passwdlen,
    const uint8_t * setting,
    uint8_t * buf, size_t buflen)
{
	uint8_t hash[HASH_SIZE];
	const uint8_t * src, * salt;
	uint8_t * dst;
	size_t prefixlen, saltlen, need;
	uint64_t N;
	uint32_t r, p;

	if (setting[0] != '$' || setting[1] != '7' || setting[2] != '$')
		return NULL;
	src = setting + 3;

	{
		uint32_t N_log2;
		if (decode64_one(&N_log2, *src))
			return NULL;
		src++;
		N = (uint64_t)1 << N_log2;
	}

	src = decode64_uint32(&r, 30, src);
	if (!src)
		return NULL;

	src = decode64_uint32(&p, 30, src);
	if (!src)
		return NULL;

	prefixlen = src - setting;

	salt = src;
	src = (uint8_t *)strrchr((char *)salt, '$');
	if (src)
		saltlen = src - salt;
	else
		saltlen = strlen((char *)salt);

	need = prefixlen + saltlen + 1 + HASH_LEN + 1;
	if (need > buflen || need < saltlen)
		return NULL;

	if (escrypt_kdf(local, passwd, passwdlen, salt, saltlen,
	    N, r, p, hash, sizeof(hash)))
		return NULL;

	dst = buf;
	memcpy(dst, setting, prefixlen + saltlen);
	dst += prefixlen + saltlen;
	*dst++ = '$';

	dst = encode64(dst, buflen - (dst - buf), hash, sizeof(hash));
	/* Could zeroize hash[] here, but escrypt_kdf() doesn't zeroize its
	 * memory allocations yet anyway. */
	if (!dst || dst >= buf + buflen) /* Can't happen */
		return NULL;

	*dst = 0; /* NUL termination */

	return buf;
}

uint8_t *
escrypt(const uint8_t * passwd, const uint8_t * setting)
{
	static uint8_t buf[4 + 1 + 5 + 5 + BYTES2CHARS(32) + 1 + HASH_LEN + 1];
	escrypt_local_t local;
	uint8_t * retval;

	if (escrypt_init_local(&local))
		return NULL;
	retval = escrypt_r(&local,
	    passwd, strlen((char *)passwd), setting, buf, sizeof(buf));
	if (escrypt_free_local(&local))
		return NULL;
	return retval;
}

uint8_t *
escrypt_gensalt_r(uint32_t N_log2, uint32_t r, uint32_t p,
    const uint8_t * src, size_t srclen,
    uint8_t * buf, size_t buflen)
{
	uint8_t * dst;
	size_t prefixlen = 3 + 1 + 5 + 5;
	size_t saltlen = BYTES2CHARS(srclen);
	size_t need;

	need = prefixlen + saltlen + 1;
	if (need > buflen || need < saltlen || saltlen < srclen)
		return NULL;

	if (N_log2 > 63 || ((uint64_t)r * (uint64_t)p >= (1U << 30)))
		return NULL;

	dst = buf;
	*dst++ = '$';
	*dst++ = '7';
	*dst++ = '$';

	*dst++ = itoa64[N_log2];

	dst = encode64_uint32(dst, buflen - (dst - buf), r, 30);
	if (!dst) /* Can't happen */
		return NULL;

	dst = encode64_uint32(dst, buflen - (dst - buf), p, 30);
	if (!dst) /* Can't happen */
		return NULL;

	dst = encode64(dst, buflen - (dst - buf), src, srclen);
	if (!dst || dst >= buf + buflen) /* Can't happen */
		return NULL;

	*dst = 0; /* NUL termination */

	return buf;
}

uint8_t *
escrypt_gensalt(uint32_t N_log2, uint32_t r, uint32_t p,
    const uint8_t * src, size_t srclen)
{
	static uint8_t buf[4 + 1 + 5 + 5 + BYTES2CHARS(32) + 1];
	return escrypt_gensalt_r(N_log2, r, p, src, srclen,
	    buf, sizeof(buf));
}

int
crypto_scrypt(const uint8_t * passwd, size_t passwdlen,
    const uint8_t * salt, size_t saltlen, uint64_t N, uint32_t r, uint32_t p,
    uint8_t * buf, size_t buflen)
{
	escrypt_local_t local;
	int retval;

	if (escrypt_init_local(&local))
		return -1;
	retval = escrypt_kdf(&local,
	    passwd, passwdlen, salt, saltlen, N, r, p, buf, buflen);
	if (escrypt_free_local(&local))
		return -1;
	return retval;
}
