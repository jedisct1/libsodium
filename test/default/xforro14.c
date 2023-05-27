
#define TEST_NAME "xforro14"
#include "cmptest.h"
typedef struct Hforro14TV_
{
    const char key[crypto_core_hforro14_KEYBYTES * 2 + 1];
    const char in[crypto_core_hforro14_INPUTBYTES * 2 + 1];
    const char out[crypto_core_hforro14_OUTPUTBYTES * 2 + 1];
} Hforro14TV;

static void
tv_hforro14(void)
{
    static const Hforro14TV tvs[] = {
        {"fb5c38aeebfbf405f0a60b2296bfdf1a07b556fbc32ccb65b7a21de1b0fb007b", "6863370d90ce0bde35aec0088feace1a", "9ccfdfdf400d0de0d5dec1229b1cea3eb6392c0d89d345bffdd0382cf39e2cf7"},
        {"18c34ee0ed3cf9bd9de5793b322a49427f57a86ac336b4ec1a1795aaeb20a252", "e362bcad548ac003aad3ba46ef1607e1", "5317d3b641940c2a471b776295f81f48668a5181b96e034e2293fbf893fca455"},
        {"b37953420d6b02c5040ea748e132df76f7d155823f5d97763461b424034e1478", "5bd935c83d133210d9de9aae9d651441", "3d198914d01242f147b9340edc1ac8ecb176696b3a69bc5ac7b51dc59040685c"},
        {"9bff51f895dd2da93864f43525eec5d62e8e83f5f5b5c682e520f6f2d7498937", "65530a0cbde7e040c891db5ff6008d2d", "1333aadaf4c90710a97b9facbbf4ac99ee30043682ffdedc1d69775be2b0ba09"},
        {"0809d203ed3571ff3aa4ec4f6fc1de10b92e87c76f4b4ffdfad8fe37381d9d67", "8aa6100dda567d04614db23e38e548db", "64230964ce353b7886adf65f128dec980df7a5ff626c284e187848336a79e7fa"},
        {"1eb598146ce623ad2f9862d71f7ac79ea7e4731b794ed28e32a8327831364309", "3ad23370a9be92f5c0e316f041becef6", "3ff7dde95bc81a01846c8a63d4c476ec82ac2b2006ef72a73948ca90cad543f9"},
        {"6b0ab6913ac0be730e2a55df94a25de7ee45df5bb12e39e82d5ecc89e06835a5", "30c3e2daf14197054dc24097747cdecc", "15951461ee9144e6ad2231711156eb537a7e29c475e7fcbe2118f0c000b05735"},
        {"342b874ca9a4103c1b4d7c1435a602fd7e6e31f3352468fb4d6d3635cf887f66", "41fa687d949788ca4305f4118d068150", "f101b37cf0b2e2af9a4fe98c098a5ccf6c8a4565647c0f30cc9c59ae6c03e1fb"},
        {"2b90f875fc53683fa2bb3c0f592f2040ed0bb3db5e63817e15312518d7d0c458", "a255722a92b7c63f1292f4fef8cb0146", "0ec210f21643955859c28cec1721424ab1bd987b1a383401d91699999c424bf2"},
        {"be875e8fd7ac02a9dd861b0f0c9be1b9c2df0dcbe131536ad0d08cde49de6e77", "7b155d82389c755f53ee85194da1c08f", "5759804ebdb0fa3e622bbddbc2490a14d6bf25211910636ec168f2a086cd4134"}};
    const Hforro14TV *tv;
    unsigned char *constant;
    unsigned char *key;
    unsigned char *in;
    unsigned char *out;
    unsigned char *out2;
    size_t i;

    key = (unsigned char *)sodium_malloc(crypto_core_hforro14_KEYBYTES);
    in = (unsigned char *)sodium_malloc(crypto_core_hforro14_INPUTBYTES);
    out = (unsigned char *)sodium_malloc(crypto_core_hforro14_OUTPUTBYTES);
    out2 = (unsigned char *)sodium_malloc(crypto_core_hforro14_OUTPUTBYTES);

    for (i = 0; i < (sizeof tvs) / (sizeof tvs[0]); i++)
    {
        tv = &tvs[i];
        sodium_hex2bin(key, crypto_core_hforro14_KEYBYTES,
                       tv->key, strlen(tv->key), NULL, NULL, NULL);
        sodium_hex2bin(in, crypto_core_hforro14_INPUTBYTES,
                       tv->in, strlen(tv->in), NULL, NULL, NULL);
        sodium_hex2bin(out, crypto_core_hforro14_OUTPUTBYTES,
                       tv->out, strlen(tv->out), NULL, NULL, NULL);
        crypto_core_hforro14(out2, in, key);
        assert(memcmp(out, out2, crypto_core_hforro14_OUTPUTBYTES) == 0);
    }

    sodium_free(out2);
    sodium_free(out);
    sodium_free(in);
    sodium_free(key);

    assert(crypto_core_hforro14_outputbytes() == crypto_core_hforro14_OUTPUTBYTES);
    assert(crypto_core_hforro14_inputbytes() == crypto_core_hforro14_INPUTBYTES);
    assert(crypto_core_hforro14_keybytes() == crypto_core_hforro14_KEYBYTES);
    assert(crypto_core_hforro14_constbytes() == crypto_core_hforro14_CONSTBYTES);

    printf("tv_hforro14: ok\n");
}

#define Xforro14_OUT_MAX 100

typedef struct Xforro14TV_
{
    const char key[crypto_stream_xforro14_KEYBYTES * 2 + 1];
    const char nonce[crypto_stream_xforro14_NONCEBYTES * 2 + 1];
    const char out[Xforro14_OUT_MAX * 2 + 1];
} Xforro14TV;

static void
tv_stream_xforro14(void)
{
    static const Xforro14TV tvs[] = {
        {"446a5599971cbbc8c4c5379f012be204d73d977af5cfdf18e1c4f6e52bb2f948", "b90ae2af0d3158b0a5625a7330e8d8837a6e3edc294986af", "508946580f96ebb499c55211826bb5e71db5563b019536182fb6bf5d4e"},
        {"5400df2a2368cd0208707b0d98be2ae1e2f86059262a633bf16385aafbb7355b", "1b04612a71d647e9afbdefa2e8b4246a9f3873a90b7a262f", "9f3619aed813470ddc1bfa648c76cd5d397451b26b92bdc579563bca"},
        {"99f2e5cd47bef485725b6e79fa942d11bfd74c064829265cd2518d12e6cce1c3", "02c8f41fa64a64b158b7e8df76ac719ffdb399adc9e47bf4", "6260681b40040499890de1e1f6daa354e862fe93a362"},
        {"102326ecaef535a79df587c1d42754b9331fc385958f69a5ddf16fe13a3ab47b", "2bbf7a342c717330d79e6c8f729d88169d109972f27c7d92", "57298545d4d906b183f360a9bb3dfd8df9f615c16d986b3bf75969a4196345278129c6f2978c17dbbb0cfdb2e39a51b539efe8b5ff7457f70f"},
        {"2002c6fd52320bcc96bb60fb947556448033c377ca6319e200e5bcd90952f418", "eeacd80edb362c44c12d4c91e9043e28f4da21b590f99ebc", "57ffc7980a93db58b9d9c50298cae4879d223ccf2cb8834a57e3d77d37b67e345d455d19bd0e72af80307794476edd45"},
        {"39df4665ba6c0c5bf34f0480640ea6490ffbebef6e7cbc8940e8c6108584351d", "5dfe6d3747bf965b8691a206100c5f5404177f2879eeeb02", "ab5d38c59f86125744910b0ad5a0e409336ad0442cb6ec2a152946aabe477a"},
        {"d130c9253c4900d662278106d726b6d2ed849767ca51765e222f1ce5f2085a84", "9b60136a07315bf14eb36748a871cb2ecaac68102a17c388", "d73838e653dc615d776cb70a04704989b8d941e370eaa0b3ab6e757ad54352a3c3a1cae5bc61"},
        {"179042a891328da594339db39828b5d3699a838a96b012d7b68976bc8d916fc3", "b6c1d879c52fd5b5ec25b87acd97e94f2ab544e5be5bf98e", "d44c4da24070042b4f3bc2160589506394d45f2645cffe90e89af5f084e658b5a334086505caf6f69d7a18e4b0ff1c90cec208e39443c6d5f168123b44b1ece426a048f16aacaf83b924d54d"},
        {"a759f38844b8ee3398360cf1d775b7ed6ab4a98ed49548681286146aad69c67c", "6bbcbd49d217cb91d0571a9230a14d3566929f03ba10f643", "62c2377bdae04f94e5ed4ae5c862a1e76b0c134a5740e54734ad8b3b98dfbb9c5356d82f556b0f965c9a2cb9186f9b77dbda0ac2"},
        {"3259b24feaa3a0c0079bef16e5df335e9a6aac9f61602d2bd736ae25f662fe31", "fda7570713c45f69559c1ca725d2bc10723ea972b1e5c77c", "21cc07f4686a8ff078089309d35e43b3668c4b7a71f4efaf09ab1d6165e07e15f26acf9346a4b6b97b53e8cdc505af2c70b9a12fd462b3b53f727eec8263381c10d2541fee69acffc06e97108ab74fd147ac9568ba1cfe8c61eb71"},
    };
    const Xforro14TV *tv;
    char *hex;
    unsigned char *key;
    unsigned char *nonce;
    unsigned char *out;
    unsigned char *out2;
    size_t out_len;
    size_t i;

    key = (unsigned char *)sodium_malloc(crypto_stream_xforro14_KEYBYTES);
    nonce = (unsigned char *)sodium_malloc(crypto_stream_xforro14_NONCEBYTES);
    out = (unsigned char *)sodium_malloc(Xforro14_OUT_MAX);
    for (i = 0; i < (sizeof tvs) / (sizeof tvs[0]); i++)
    {
        tv = &tvs[i];

        sodium_hex2bin(key, crypto_stream_xforro14_KEYBYTES,
                       tv->key, strlen(tv->key), NULL, NULL, NULL);
        sodium_hex2bin(nonce, crypto_stream_xforro14_NONCEBYTES,
                       tv->nonce, strlen(tv->nonce), NULL, NULL, NULL);
        sodium_hex2bin(out, Xforro14_OUT_MAX,
                       tv->out, strlen(tv->out), NULL, &out_len, NULL);
        out2 = (unsigned char *)sodium_malloc(out_len);
        crypto_stream_xforro14(out2, out_len, nonce, key);
        assert(memcmp(out, out2, out_len) == 0);
        crypto_stream_xforro14_xor(out2, out, out_len, nonce, key);
        assert(sodium_is_zero(out2, out_len));
        crypto_stream_xforro14_xor_ic(out2, out, out_len, nonce, 0, key);
        assert(sodium_is_zero(out2, out_len));
        crypto_stream_xforro14_xor_ic(out2, out, out_len, nonce, 1, key);
        assert(!sodium_is_zero(out2, out_len));
        crypto_stream_xforro14_xor(out, out, out_len, nonce, key);
        assert(sodium_is_zero(out, out_len));
        sodium_free(out2);
    }

    out2 = (unsigned char *)sodium_malloc(0);
    crypto_stream_xforro14(out2, 0, nonce, key);
    crypto_stream_xforro14_xor(out2, out2, 0, nonce, key);
    crypto_stream_xforro14_xor_ic(out2, out2, 0, nonce, 1, key);
    sodium_free(out2);
    sodium_free(out);

    out = (unsigned char *)sodium_malloc(64);
    out2 = (unsigned char *)sodium_malloc(128);
    randombytes_buf(out, 64);
    randombytes_buf(out2, 64);
    memcpy(out2 + 64, out, 64);
    crypto_stream_xforro14_xor_ic(out, out, 64, nonce, 1, key);
    crypto_stream_xforro14_xor(out2, out2, 128, nonce, key);
    assert(memcmp(out, out2 + 64, 64) == 0);
    sodium_free(out);
    sodium_free(out2);

    out = (unsigned char *)sodium_malloc(192);
    out2 = (unsigned char *)sodium_malloc(192);
    memset(out, 0, 192);
    memset(out2, 0, 192);
    crypto_stream_xforro14_xor_ic(out2, out2, 192, nonce,
                                  (1ULL << 32) - 1ULL, key);
    crypto_stream_xforro14_xor_ic(out, out, 64, nonce,
                                  (1ULL << 32) - 1ULL, key);
    crypto_stream_xforro14_xor_ic(out + 64, out + 64, 64, nonce,
                                  (1ULL << 32), key);
    crypto_stream_xforro14_xor_ic(out + 128, out + 128, 64, nonce,
                                  (1ULL << 32) + 1, key);
    assert(memcmp(out, out2, 192) == 0);
    hex = (char *)sodium_malloc(192 * 2 + 1);
    sodium_bin2hex(hex, 192 * 2 + 1, out, 192);
    printf("%s\n", hex);

    memset(key, 0, crypto_stream_xforro14_KEYBYTES);
    crypto_stream_xforro14_keygen(key);
    assert(sodium_is_zero(key, crypto_stream_xforro14_KEYBYTES) == 0);

    sodium_free(hex);
    sodium_free(out);
    sodium_free(out2);

    sodium_free(nonce);
    sodium_free(key);

    assert(crypto_stream_xforro14_keybytes() == crypto_stream_xforro14_KEYBYTES);
    assert(crypto_stream_xforro14_noncebytes() == crypto_stream_xforro14_NONCEBYTES);
    assert(crypto_stream_xforro14_messagebytes_max() == crypto_stream_xforro14_MESSAGEBYTES_MAX);

    printf("tv_stream_xforro14: ok\n");
}

int
main(void)
{
    tv_hforro14();
    tv_stream_xforro14();

    return 0;
}
