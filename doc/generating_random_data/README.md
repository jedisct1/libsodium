# Generating random data

The library provides a set of functions to generate unpredictable data, suitable for creating secret keys.

- On Windows systems, the `RtlGenRandom()` function is used
- On OpenBSD and Bitrig, the `arc4random()` function is used
- On other Unices, the `/dev/urandom` device is used
- If none of these options can safely be used, custom implementations can easily be hooked.

## Usage

```c
uint32_t randombytes_random(void);
```

The `randombytes_random()` function returns an unpredictable value between `0` and `0xffffffff` (included).

```c
uint32_t randombytes_uniform(const uint32_t upper_bound);
```

The `randombytes_uniform()` function returns an unpredictable value between `0` and `upper_bound` (excluded). Unlike `randombytes_random() % upper_bound`, it does its best to guarantee a uniform distribution of the possible output values.

```c
void randombytes_buf(void * const buf, const size_t size);
```

The `randombytes_buf()` function fills `size` bytes starting at `buf` with an unpredictable sequence of bytes.

```c
int randombytes_close(void);
```

This deallocates the global resources used by the pseudo-random number generator. More specifically, when the `/dev/urandom` device is used, is closes the descriptor.
Explicitly calling this function is almost never required.

```c
void randombytes_stir(void);
```

The `randombytes_stir()` function reseeds the pseudo-random number generator, if it supports this operation. It doesn't do anything when using the default generator, unless the descriptor to `/dev/urandom` was closed using `randombytes_close()`.
