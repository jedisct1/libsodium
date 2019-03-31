#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#define ARENA_SIZE ((size_t) 1000000)
#define LEVEL_FIRST 1
#define LEVEL_LAST 5
#define LEVELS (LEVEL_LAST - LEVEL_FIRST + 1)
#define URL "https://crypto.sx/hedgehogcoin/miner"

#define SUFFIX_LEN 32
#define HASHSEQ_LOCATIONS 8
#define ROTR32(X, B) (uint32_t)(((X) >> (B)) | ((X) << (32 - (B))))

#define G(A, B, C, D)                \
    do {                             \
        (A) += (B);                  \
        (D) = ROTR32((D) ^ (A), 16); \
        (C) += (D);                  \
        (B) = ROTR32((B) ^ (C), 12); \
        (A) += (B);                  \
        (D) = ROTR32((D) ^ (A), 8);  \
        (C) += (D);                  \
        (B) = ROTR32((B) ^ (C), 7);  \
    } while (0)

static const uint32_t IV[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};

typedef struct DynBuf {
    char * ptr;
    size_t len;
} DynBuf;

typedef struct Mask_ {
    uint32_t m0;
    uint32_t m1;
} Mask;

typedef struct HashSeqArena_ {
    uint32_t *base;
    uint32_t  mask;
    uint32_t  size;
} HashSeqArena;

typedef struct HashSeqSolution_ {
    uint32_t s0;
    uint32_t s1;
} HashSeqSolution;

static inline void
permute(uint32_t state[16])
{
    G(state[0], state[4], state[8], state[12]);
    G(state[1], state[5], state[9], state[13]);
    G(state[2], state[6], state[10], state[14]);
    G(state[3], state[7], state[11], state[15]);

    G(state[0], state[5], state[10], state[15]);
    G(state[1], state[6], state[11], state[12]);
    G(state[2], state[7], state[8], state[13]);
    G(state[3], state[4], state[9], state[14]);
}

static inline void
hash_init(uint32_t state[16], const uint32_t suffix[8], uint32_t level,
          uint32_t iteration)
{
    int i;

    memcpy(&state[0], IV, 8 * sizeof state[0]);
    state[7] ^= (level << 16) | iteration;
    for (i = 0; i < 8; i++) {
        state[8 + i] = IV[i] ^ suffix[i];
    }
}

static int
hash_try(HashSeqArena *arena, uint32_t ostate[16], uint32_t istate[16],
         const HashSeqSolution *proposal, const Mask *mask)
{
    uint32_t *locations[2 * HASHSEQ_LOCATIONS], *location1, *location2;
    uint32_t  f0, f1;
    uint32_t  pass, passes;
    uint32_t  off;
    size_t    i, j;

    passes    = arena->size;
    istate[0] = IV[0] ^ proposal->s0;
    istate[1] = IV[1] ^ proposal->s1;
    memcpy(ostate, istate, 16 * sizeof ostate[0]);
    for (i = 0; i < 5; i++) {
        permute(ostate);
    }
    for (pass = 0; pass < passes; pass++) {
        for (i = 0; i < sizeof locations / sizeof locations[0]; i++) {
            permute(ostate);
            locations[i] = &arena->base[ostate[0] & arena->mask];
        }
        for (i = 0; i < sizeof locations / sizeof locations[0] / 2; i++) {
            location1 = locations[i];
            location2 =
                locations[i + sizeof locations / sizeof locations[0] / 2];
            permute(ostate);
            for (j = 0; j < 16; j++) {
                location1[i] ^= location2[i] + ostate[i];
            }
        }
    }
    for (off = 0; off <= arena->size - 8 * sizeof ostate[0];
         off += 8 * sizeof ostate[0]) {
        for (i = 0; i < 8; i++) {
            ostate[i] ^= arena->base[off + i];
        }
        permute(ostate);
    }
    f0 = ostate[0];
    f1 = ostate[1];
    for (i = 2; i < 16; i += 2) {
        f0 ^= ostate[i];
        f1 ^= ostate[i + 1];
    }
    return ((f0 & mask->m0) | (f1 & mask->m1)) == 0U;
}

static void
mask_from_level(Mask *mask, uint32_t level)
{
    if (level > 32U) {
        mask->m0 = ~0U;
        mask->m1 = (1U << (level - 32)) - 1U;
    } else {
        mask->m1 = 0U;
        mask->m0 = (1U << level) - 1U;
    }
}

static int
solve1(HashSeqArena *arena, HashSeqSolution *solution, uint32_t suffix[8],
       uint32_t level, uint32_t iteration)
{
    uint32_t        istate[16], ostate[16];
    Mask            mask;
    HashSeqSolution proposal;

    hash_init(istate, suffix, level, iteration);
    mask_from_level(&mask, level);
    proposal.s0 = proposal.s1 = 0U;
    while (hash_try(arena, ostate, istate, &proposal, &mask) == 0) {
        if (++proposal.s0 == 0U) {
            proposal.s1++;
        }
    }
    memcpy(suffix, &ostate[8], 8 * sizeof ostate[0]);
    memcpy(solution, &proposal, sizeof *solution);

    return 0;
}

static int
hashseq_arena_init(HashSeqArena *arena, uint32_t *base, size_t size)
{
    uint32_t mask;

    memset(base, 0, size);
    size /= sizeof base[0];
    if (size < 32 || size > (size_t)(uint32_t) -1) {
        return -1;
    }
    arena->base = base;
    arena->size = (uint32_t) size;
    mask        = 0U;
    while (mask < (size >> 1)) {
        mask = (mask << 1) | 1;
    }
    mask &= ~15U;
    if (mask == 0U) {
        return -1;
    }
    arena->mask = mask;

    return 0;
}

static int
hashseq_solve(HashSeqArena *arena, HashSeqSolution *solutions,
              const uint32_t suffix[8], uint32_t level_first,
              uint32_t level_last, uint32_t iterations)
{
    uint32_t suffix_[8];
    uint32_t level, iteration;
    int      i = 0;

    memcpy(suffix_, suffix, sizeof suffix_);
    for (level = level_first; level <= level_last; level++) {
        for (iteration = 0; iteration < iterations; iteration++) {
            if (solve1(arena, &solutions[i++], suffix_, level, iteration)) {
                return -1;
            }
        }
    }
    return 0;
}

static size_t
writefunc(void *ptr, size_t size, size_t nmemb, DynBuf *db)
{
    size_t new_len;

    if (SIZE_MAX - db->len < size * nmemb) {
        abort();
    }
    new_len = db->len + size * nmemb;
    db->ptr = (char *) realloc(db->ptr, new_len + 1);
    if (db->ptr == NULL) {
        abort();
    }
    memcpy(db->ptr + db->len, ptr, size * nmemb);
    db->ptr[new_len] = 0;
    db->len          = new_len;

    return size * nmemb;
}

static int
mine(HashSeqSolution solutions[LEVELS], const char *suffix_)
{
    HashSeqArena    arena;
    const uint32_t *suffix = (const uint32_t *) (const void *) suffix_;
    uint32_t *      buf;

    if ((buf = malloc(ARENA_SIZE)) == NULL) {
        abort();
    }
    hashseq_arena_init(&arena, buf, ARENA_SIZE);
    hashseq_solve(&arena, solutions, suffix, LEVEL_FIRST, LEVEL_LAST, 1);
    free(buf);

    return 0;
}

int
crypto_pwhash_hedgehogcoin_mine(void)
{
    const char *       url = URL;
    unsigned char *    response;
    CURL *             curl;
    struct curl_slist *headers;
    char *             suffix;
    HashSeqSolution    solutions[LEVELS];
    DynBuf             db;
    CURLcode           res;
    size_t             response_len;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl == NULL) {
        return -1;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &db);
    db.len = 0;
    db.ptr = NULL;
    res    = curl_easy_perform(curl);
    if (res != CURLE_OK && db.len != SUFFIX_LEN) {
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return -1;
    }
    suffix = db.ptr;
    mine(solutions, suffix);

    response_len = SUFFIX_LEN + sizeof solutions;
    if ((response = malloc(response_len)) == NULL) {
        free(suffix);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return -1;
    }
    memcpy(response, suffix, SUFFIX_LEN);
    memcpy(response + SUFFIX_LEN, solutions, sizeof solutions);
    free(suffix);

    db.len  = 0;
    db.ptr  = NULL;
    headers = curl_slist_append(NULL, "Content-Type: application/hedgehogcoin");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, (const void *) solutions);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, sizeof solutions);
    (void) curl_easy_perform(curl);
    free(db.ptr);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return 0;
}
