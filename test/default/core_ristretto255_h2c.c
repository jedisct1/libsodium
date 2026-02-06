#define TEST_NAME "core_ristretto255_h2c"
#include "cmptest.h"

typedef struct TestData_ {
    int         ro;         //believe this makes no difference at the moment.
    const char *msg;
    const size_t msg_len;
    const char  tv_blind[64 + 1];
    const char  tv_blinded[64 + 1];
    const char  tv_y[64 + 1];
    const char *tv_ctx_str;
    const size_t tv_ctx_str_len;
} TestData;

/*
This test data is taken from RFC9497, which specifies an OPRF.  The process overview is:
 1. Hash to Curve on an a string + context to get an element
 2. multiply the element by a blinding scalar to get a blinded element
 3. (do stuff with the blinded element we don't really care about, including: send to remote party for them to further muliply by their secret scalar and return to the original user to unblind)


The inputs to step 1, the blinding factor used as input to step 2 and 2 outputs from step 2 are given as test vectors by the RFC.
With a little work to unblind the output of step 2 using the inverse of the blinding scalar it can be transformed into a minimal test for H2C.

However, traceability is important for confidence, so the actual test values from the RFC are given below, along with the pre-calculated unblinded element,
and the test code dynamically unblinds and compares the results to ensure all outputs match.
*/
static TestData test_data[] = {
        {
            0,
            "00",1,                                                             //msg input (from RFC9497)
            "64d37aed22a27f5191de1c1d69fadb899d8862b58eb4220029e036ec4c1f6706", //blinding factor (scalar) (from RFC9497)
            "609a0ae68c15a3cf6903766461307e5c8bb2f95e7e6550e1ffa2dc99e412803c", //blinded element (from RFC9497)
            "5873db2e5f8f4f544ce3e574c74c487f03bc64a2cf63b7c913908091aab03357", //unblinded element (calculated from above values)
            "HashToGroup-OPRFV1-\x00-ristretto255-SHA512",                      //context string
            40U                                                                 //context string length
        },
        {
            0,
            "5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a",17,                            //msg input (from RFC9497)
            "64d37aed22a27f5191de1c1d69fadb899d8862b58eb4220029e036ec4c1f6706", //blinding factor (scalar) (from RFC9497)
            "da27ef466870f5f15296299850aa088629945a17d1f5b7f5ff043f76b3c06418", //blinded element (from RFC9497)
            "743d49d207339ae67aef8f4d0777744e5a604b94df5cbcc13e3dd87e79985a39", //unblinded element (calculated from above values)
            "HashToGroup-OPRFV1-\x00-ristretto255-SHA512",                        //context string
            40U                                                                 //context string length
        },
        //VOPRF mode:
        {
            0,
            "00",1,                                                             //msg input (from RFC9497)
            "64d37aed22a27f5191de1c1d69fadb899d8862b58eb4220029e036ec4c1f6706", //blinding factor (scalar) (from RFC9497)
            "863f330cc1a1259ed5a5998a23acfd37fb4351a793a5b3c090b642ddc439b945", //blinded element (from RFC9497)
            "868c9140811d0dc38291c7bbc0bd8f301d0d4e8b15f65e442184a233b8791703", //unblinded element (calculated from above values)
            "HashToGroup-OPRFV1-\x01-ristretto255-SHA512",                      //context string
            40U                                                                 //context string length
        },
        {
            0,
            "5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a",17,                            //msg input (from RFC9497)
            "64d37aed22a27f5191de1c1d69fadb899d8862b58eb4220029e036ec4c1f6706", //blinding factor (scalar) (from RFC9497)
            "cc0b2a350101881d8a4cba4c80241d74fb7dcbfde4a61fde2f91443c2bf9ef0c", //blinded element (from RFC9497)
            "caff66fcdc41da4d87ccc72aaac70c6e267a4b55c3dc9489bb365a70a04f1a52", //unblinded element (calculated from above values)
            "HashToGroup-OPRFV1-\x01-ristretto255-SHA512",                      //context string
            40U                                                                 //context string length
        },
        //POPRF mode:
        {
            0,
            "00",1,                                                             //msg input (from RFC9497)
            "64d37aed22a27f5191de1c1d69fadb899d8862b58eb4220029e036ec4c1f6706", //blinding factor (scalar) (from RFC9497)
            "c8713aa89241d6989ac142f22dba30596db635c772cbf25021fdd8f3d461f715", //blinded element (from RFC9497)
            "5e121666a7c49e1f24b59fef212aa9c8556e096d3f1598cc02f5c70215b32d73", //unblinded element (calculated from above values)
            "HashToGroup-OPRFV1-\x02-ristretto255-SHA512",                      //context string
            40U                                                                 //context string length
        },
        {
            0,
            "5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a5a",17,                            //msg input (from RFC9497)
            "64d37aed22a27f5191de1c1d69fadb899d8862b58eb4220029e036ec4c1f6706", //blinding factor (scalar) (from RFC9497)
            "f0f0b209dd4d5f1844dac679acc7761b91a2e704879656cb7c201e82a99ab07d", //blinded element (from RFC9497)
            "785596d91746f8ff32f9f95af448408435284e5935932572caa9e7e0b04fdb03", //unblinded element (calculated from above values)
            "HashToGroup-OPRFV1-\x02-ristretto255-SHA512",                      //context string
            40U                                                                 //context string length
        }
    };
//NB: Batched test vectors given by the RFC are duplicates of the individual inputs (albeit sometimes with a different blind) consequently these are not further tests of the H2C function as they hash to the same elements as already in the tests above.

#define H2CHASH crypto_core_ristretto255_H2CSHA512


void reverse_byte_order(unsigned char *output, const unsigned char *input, const size_t len)
{
        for (size_t j = 0; j < len; j++) {
            output[j] = input[len - 1U - j];
        }
}


#define max(a,b) ( a < b ? b : a )

void from_test_hex(unsigned char *reverse_buf,unsigned char *forward_buf,const char* source, const size_t len,int reverse)
{
        sodium_hex2bin(reverse_buf, len,
                       source, (size_t) -1U, NULL, NULL, NULL);
        if(reverse)
            reverse_byte_order(forward_buf,reverse_buf,len);
        else
            memcpy(forward_buf,reverse_buf,len);
}

int
main(void)
{
    unsigned char *tv_blindr, *tv_blind; //bin versions of hex test values.
    unsigned char *tv_blindedr, *tv_blinded;
    unsigned char *tv_yr, *tv_y, *y, *blinded_y;

    unsigned char *blind_inverse;
    unsigned char *calculated_y;

    char *         msg, *msgr;

    char *         expected_elem_hex, *calc_elem_hex, *scalar_hex;
    char *         oversized_ctx;
    size_t         i;
    size_t         oversized_ctx_len = 500U;

    const char *test_ctx_str;
    size_t test_ctx_len;


    //TODO: check on deallocation!
    tv_blindr = (unsigned char *) sodium_malloc(crypto_core_ristretto255_SCALARBYTES);
    tv_blind  = (unsigned char *) sodium_malloc(crypto_core_ristretto255_SCALARBYTES);

    tv_blindedr = (unsigned char *) sodium_malloc(crypto_core_ristretto255_BYTES);
    tv_blinded  = (unsigned char *) sodium_malloc(crypto_core_ristretto255_BYTES);

    tv_yr = (unsigned char *) sodium_malloc(crypto_core_ristretto255_BYTES);
    tv_y  = (unsigned char *) sodium_malloc(crypto_core_ristretto255_BYTES);

    //calculated values:
    blind_inverse = (unsigned char *) sodium_malloc(crypto_core_ristretto255_SCALARBYTES);
    calculated_y  = (unsigned char *) sodium_malloc(crypto_core_ristretto255_BYTES);
    y             = (unsigned char *) sodium_malloc(crypto_core_ristretto255_BYTES);
    blinded_y     = (unsigned char *) sodium_malloc(crypto_core_ristretto255_BYTES);


    //output values:
    expected_elem_hex = (char *) sodium_malloc(crypto_core_ristretto255_BYTES * 2U + 1U);
    calc_elem_hex = (char *) sodium_malloc(crypto_core_ristretto255_BYTES * 2U + 1U);
    scalar_hex = (char *) sodium_malloc(crypto_core_ristretto255_SCALARBYTES * 2U + 1U);

    size_t max_len = 0;
    for (i = 0U; i < (sizeof test_data) / (sizeof test_data[0]); i++) {
            max_len = max(max_len,test_data[i].msg_len);
    }

    msgr = (unsigned char *) sodium_malloc(max_len);
    msg = (unsigned char *) sodium_malloc(max_len);

    int success;

    for (i = 0U; i < (sizeof test_data) / (sizeof test_data[0]); i++) {
        success = 1;
        //convert hex strings to bin:
        //matched the form of ed25519 tests made into a function to simplify.
        from_test_hex( tv_blindr,     tv_blind,    test_data[i].tv_blind,     crypto_core_ristretto255_SCALARBYTES, 0 );
        from_test_hex( tv_blindedr,   tv_blinded,  test_data[i].tv_blinded,   crypto_core_ristretto255_BYTES      , 0 );
        from_test_hex( tv_yr,         tv_y,        test_data[i].tv_y,         crypto_core_ristretto255_BYTES      , 0 );
        from_test_hex( msgr,          msg,         test_data[i].msg,          test_data[i].msg_len                , 0 );

        test_ctx_str = test_data[i].tv_ctx_str;
        test_ctx_len = test_data[i].tv_ctx_str_len;

        if(1)
        {
            printf("Test #%u:\n", (unsigned) i);

            printf("\tRFC9497 Test Vector Specified Values:\n");
            sodium_bin2hex(scalar_hex, crypto_core_ristretto255_SCALARBYTES * 2U + 1U, tv_blind, crypto_core_ristretto255_SCALARBYTES);
            printf("\t\tBlind:                  %s\n",scalar_hex);

            sodium_bin2hex(calc_elem_hex, crypto_core_ristretto255_BYTES * 2U + 1U, tv_blinded, crypto_core_ristretto255_BYTES);
            printf("\t\tBlinded Element:        %s\n",calc_elem_hex);

            printf("\tPrecalculated value from the above:\n");
            sodium_bin2hex(calc_elem_hex, crypto_core_ristretto255_BYTES * 2U + 1U, tv_y, crypto_core_ristretto255_BYTES);
            printf("\t\tExpected H2C element:   %s\n",calc_elem_hex);
        }

        //1. unblind the test vector and confirm that we get what we expect
            //1.1 calculate the unblinding factor:
            if(crypto_core_ristretto255_scalar_invert(blind_inverse, tv_blind) != 0)
            {
                printf("Error: could not calculate the inverse of the blind!\n");
                success = 0;
                continue;
            }
            //1.2. apply the unblinding factor:
            if(crypto_scalarmult_ristretto255(calculated_y, blind_inverse, tv_blinded) != 0)
            {
                printf("Error: could not apply the inverted the blinding factor!\n");
                success = 0;
                continue;
            }

            printf("\tDynamically calculated:\n");
            sodium_bin2hex(calc_elem_hex, crypto_core_ristretto255_BYTES * 2U + 1U, calculated_y, crypto_core_ristretto255_BYTES);
            printf("\t\tUnblinded H2C element:  %s\n",calc_elem_hex);

            //1.3. ensure that the calculated unblinded element matches the pre-calculated unblinded element
            if (memcmp(calculated_y, tv_y, crypto_core_ristretto255_BYTES) != 0) {
                printf("Error: the calculated unblinded element doesn't match the pre-calculated unblinded element!\n");
                success = 0;
            }

        //2. execute h2c and confirm we get what we expect << this is the test we really want to do, but we also want to start from published reference values,
            // so section 1. is the documentation on how we go from the published reference data to the actual data we test with.
            if(crypto_core_ristretto255_from_string(y,test_ctx_str,test_ctx_len, msg, test_data[i].msg_len, crypto_core_ristretto255_H2CSHA512) != 0) {
                printf("Error: couldn't execute h2c on this input\n");
                success = 0;
                continue;
            }

        //ensure highest bit is not set. (necessary?)
            y[crypto_core_ristretto255_BYTES - 1U] &= 0x7f;

            sodium_bin2hex(calc_elem_hex, crypto_core_ristretto255_BYTES * 2U + 1U, y, crypto_core_ristretto255_BYTES);
            printf("\t\tHashed H2C element:     %s\n",calc_elem_hex);


        if (memcmp(y, calculated_y, crypto_core_ristretto255_BYTES) != 0) {
            success = 0;

            if (memcmp(y, tv_y, crypto_core_ristretto255_BYTES) == 0) {
                //hashed doesn't match calculated, but does match pre-calculated -> calculated is probably wrong for some reason.
                sodium_bin2hex(expected_elem_hex, crypto_core_ristretto255_BYTES * 2U + 1U,
                               calculated_y, crypto_core_ristretto255_BYTES);
                sodium_bin2hex(calc_elem_hex, crypto_core_ristretto255_BYTES * 2U + 1U, y,
                               crypto_core_ristretto255_BYTES);
                printf("Test #%u failed - expected [%s] got [%s], but that does match pre-computed value.\n", (unsigned) i,
                       expected_elem_hex, calc_elem_hex);
            }
            else {
                //hashed doesn't match anything.
                sodium_bin2hex(expected_elem_hex, crypto_core_ristretto255_BYTES * 2U + 1U,
                               tv_y, crypto_core_ristretto255_BYTES);
                sodium_bin2hex(calc_elem_hex, crypto_core_ristretto255_BYTES * 2U + 1U, y,
                               crypto_core_ristretto255_BYTES);
                printf("Test #%u failed - expected [%s] got [%s]\n", (unsigned) i,
                       expected_elem_hex, calc_elem_hex);
            }
        }
        else if (memcmp(y, tv_y, crypto_core_ristretto255_BYTES) != 0) {
            success = 0;

            //hashed matches calculated, but does not match pre-calculated. -> could input or pre-calculated values have been corrupted?
            sodium_bin2hex(expected_elem_hex, crypto_core_ristretto255_BYTES * 2U + 1U,
                           tv_y, crypto_core_ristretto255_BYTES);
            sodium_bin2hex(calc_elem_hex, crypto_core_ristretto255_BYTES * 2U + 1U, y,
                           crypto_core_ristretto255_BYTES);
            printf("Test #%u failed - expected [%s] got [%s], but that does match dynamically-computed value.\n", (unsigned) i,
                   expected_elem_hex, calc_elem_hex);
        }


        //3. For completeness, blind this element and ensure it is the blinded element: (probably a bit extreme - not what the test is about), but easy to delete if not wanted.
            if(crypto_scalarmult_ristretto255(blinded_y, tv_blind, y) != 0)
            {
                //TODO: improve error message and handling:
                printf("Error: could not apply the inverted the blinding factor!\n");
                success = 0;
            }

        //if output doesn't match, report failure values.
        if (memcmp(blinded_y, tv_blinded, crypto_core_ristretto255_BYTES) != 0) {
            success = 0;
            sodium_bin2hex(expected_elem_hex, crypto_core_ristretto255_BYTES * 2U + 1U,
                           calculated_y, crypto_core_ristretto255_BYTES);
            sodium_bin2hex(calc_elem_hex, crypto_core_ristretto255_BYTES * 2U + 1U, y,
                           crypto_core_ristretto255_BYTES);
            printf("Test #%u failed - expected [%s] got [%s]\n", (unsigned) i,
                   expected_elem_hex, calc_elem_hex);
        }
        else
        {
            sodium_bin2hex(calc_elem_hex, crypto_core_ristretto255_BYTES * 2U + 1U,
                           blinded_y, crypto_core_ristretto255_BYTES);
            printf("\t\tBlinded H2C element:    %s\n",calc_elem_hex);
        }

        printf("Test [%u] outcome:   %s\n\n\n",(unsigned) i,(success ? "Success" : "Fail" ));

/*
        //Not sure if this is at all useful in this context, so left as a discussion point.

        //if RO is false
        if (test_data[i].ro == 0) {
            //use NU form
            if (crypto_core_ristretto255_from_string(
                    y, test_ctx_str, test_ctx_len,
                    (const unsigned char *) test_data[i].msg,
                    strlen(test_data[i].msg), H2CHASH) != 0) {
                printf("crypto_core_ristretto255_from_string() failed\n");
            }
        } else {
            // else use RO form.
            if (crypto_core_ristretto255_from_string_ro(
                    y, test_ctx_str, test_ctx_len,
                    (const unsigned char *) test_data[i].msg,
                    strlen(test_data[i].msg), H2CHASH) != 0) {
                printf("crypto_core_ristretto255_from_string_ro() failed\n");
            }
        }

        //if output doesn't match, report failure values.
        if (memcmp(y, tv_y, crypto_core_ristretto255_BYTES) != 0) {
            sodium_bin2hex(expected_y_hex, crypto_core_ristretto255_BYTES * 2U + 1U,
                           tv_y, crypto_core_ristretto255_BYTES);
            sodium_bin2hex(y_hex, crypto_core_ristretto255_BYTES * 2U + 1U, y,
                           crypto_core_ristretto255_BYTES);
            printf("Test #%u failed - expected [%s] got [%s]\n", (unsigned) i,
                   expected_y_hex, y_hex);
        }
*/
    }


    //edge cases:
    if(0) // disabled for establishing operational.
    {
            //test when no context given
        if (crypto_core_ristretto255_from_string(y, NULL, 0, (const unsigned char *) "msg",
                                            3U, H2CHASH) != 0 ||
            crypto_core_ristretto255_from_string(y, "", 0, guard_page, 0U, H2CHASH) != 0 ||
            crypto_core_ristretto255_from_string_ro(
                y, NULL, 0, (const unsigned char *) "msg", 3U, H2CHASH) != 0 ||
            crypto_core_ristretto255_from_string_ro(y, "", 0, guard_page, 0U,
                                               H2CHASH) != 0) {
            printf("Failed with empty parameters");
        }

            //test with an oversized context
        oversized_ctx = (char *) sodium_malloc(oversized_ctx_len);
        memset(oversized_ctx, 'X', oversized_ctx_len - 1U);
        oversized_ctx[oversized_ctx_len - 1U] = 0;
        crypto_core_ristretto255_from_string(y, oversized_ctx, strlen(oversized_ctx),
                                        (const unsigned char *) "msg", 3U,
                                        H2CHASH);
        sodium_bin2hex(calc_elem_hex, crypto_core_ristretto255_BYTES * 2U + 1U, y,
                       crypto_core_ristretto255_BYTES);
        printf("NU with oversized context: %s\n", calc_elem_hex);
        crypto_core_ristretto255_from_string_ro(y, oversized_ctx, strlen(oversized_ctx),
                                           (const unsigned char *) "msg", 3U,
                                           H2CHASH);
        sodium_bin2hex(calc_elem_hex, crypto_core_ristretto255_BYTES * 2U + 1U, y,
                       crypto_core_ristretto255_BYTES);
        printf("RO with oversized context: %s\n", calc_elem_hex);

        sodium_free(oversized_ctx);
    }

    sodium_free(msg);
    sodium_free(msgr);
    sodium_free(scalar_hex);
    sodium_free(calc_elem_hex);
    sodium_free(expected_elem_hex);
    sodium_free(blinded_y);
    sodium_free(y);
    sodium_free(calculated_y);
    sodium_free(blind_inverse);
    sodium_free(tv_y);
    sodium_free(tv_yr);
    sodium_free(tv_blinded);
    sodium_free(tv_blindedr);

    sodium_free(tv_blind);
    sodium_free(tv_blindr);

    printf("OK\n");

    return 0;
}



/*
can test:
    blinded_element -> unblinded_element matches expected
    input -> h2c  matches calcualted expected value

function specified_blind(msg,blind) {
  let px = crypto_core_ristretto255_from_string(getCTX(),msg,crypto_core_ristretto255_H2CSHA512);
  let blinded_element = crypto_scalarmult_ristretto255(blind,px);

  return blinded_element
}

*/
