# Helpers

## Constant-time comparison

```c
int sodium_memcmp(const void * const b1_, const void * const b2_, size_t len);
```

When a comparison involes secret data (e.g. key, authentication tag), is it critical to use a constant-time comparison function in order to mitigate side-channel attacks.

The `sodium_memcmp()` function can be used for this purpose.

It returns `0` is `len` bytes pointed by `b1_` are matching `len` bytes pointed by `b2_`.
It returns `-1` if they differ.

## Zeroing memory

```c
void sodium_memzero(void * const pnt, const size_t len);
```

After usage, sensitive data should be overwritten. But `memset()` and hand-written code can be silently stripped out by an optimizing compiler or by the linker.

The `sodium_memzero()` function tries to effectively zero `len` bytes starting at `pnt`, even if optimizations are being applied to the code.

## Locking memory

```c
int sodium_mlock(void * const addr, const size_t len);
```

The `sodium_mlock()` function locks at least `len` bytes of memory starting at `addr`. This can help to avoid sensitive data to be swapped to disk.

However, it is recommended to totally disable swap partitions on machines processing senstive data, or, as a second choice, to use encrypted swap partitions.

`sodium_mlock()` wraps `mlock()` and `VirtualLock()`.

```c
int sodium_munlock(void * const addr, const size_t len);
```

The `sodium_munlock()` function should be called after locked memory is not being used any more.
It will zero `len` bytes starting at `addr` before actually flagging the pages as swappable again. Calling `sodium_memzero()` prior to `sodium_munlock()` is thus not required.

## Hexadecimal encoding/decoding

```c
char *sodium_bin2hex(char * const hex, const size_t hex_maxlen,
                     const unsigned char * const bin, const size_t bin_len);
```

The `sodium_bin2hex()` function converts `bin_len` bytes stored at `bin` into a hexadecimal string.

The string is stored into `hex` and includes a nul byte (`\0`) terminator.

`hex_maxlen` is the maximum number of bytes that the function is allowed to write starting at `hex`. It should be at least `bin_len * 2 + 1`.

The function returns `hex` on success, or `NULL` on overflow.

```c
int sodium_hex2bin(unsigned char * const bin, const size_t bin_maxlen,
                   const char * const hex, const size_t hex_len,
                   const char * const ignore, size_t * const bin_len,
                   const char ** const hex_end);
```

The `sodium_hex2bin()` function parses a hexadecimal string `hex` and converts it to a byte sequence.

`hex` do not have to be nul terminated, as the number of characters to parse is supplied via the `hex_len` parameter.

`ignore` is a string of characters to skip. For example, the string `": "` allows columns and spaces to be present at any locations in the hexadecimal string. These characters will just be ignored. As a result, `"69:FC"`, `"69 FC"`, `"69 : FC"` and `"69FC"` will be valid inputs, and will produce the same output.

`ignore` can be set to `NULL` in order to disallow any non-hexadecimal character.

`bin_maxlen` is the maximum number of bytes to put into `bin`.

The parser stops when a non-hexadecimal, non-ignored character is found or when `bin_maxlen` bytes have been written.

The function returns `-1` if more than `bin_maxlen` would be required to parse the string.
It returns `0` on success, and set `hex_end`, if it is not `NULL`, to a pointer to the character following the last parsed character.
