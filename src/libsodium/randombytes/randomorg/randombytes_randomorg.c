
#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "randombytes.h"
#include "randombytes_randomorg.h"
#include "utils.h"

typedef struct dynbuf {
    char   *ptr;
    size_t  len;
} dynbuf;

static size_t
writefunc(void *ptr, size_t size, size_t nmemb, dynbuf *db)
{
    size_t new_len;

    new_len = db->len + size * nmemb;
    if (new_len < db->len) { /* might work */
        abort();
    }
    db->ptr = (char *) realloc(db->ptr, new_len + 1);
    if (db->ptr == NULL) {
        abort();
    }
    memcpy(db->ptr + db->len, ptr, size * nmemb);
    db->ptr[new_len] = 0;
    db->len = new_len;

    return size * nmemb;
}

static void
randombytes_randomorg_buf(void * const buf, const size_t size)
{
    char      url[512];
    dynbuf    db;
    CURL     *curl;
    CURLcode  res;

    db.len = 0;
    if ((db.ptr = (char *) malloc(1)) == NULL) {
        abort();
    }
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl == NULL) {
        memset(buf, 0, size); /* Be robust to network failures */
        return;
    }
    snprintf(url, sizeof url,
             "https://www.random.org/integers/?num=%llu&min=0&max=255&col=1&base=10&format=plain&rnd=new",
             (unsigned long long) size);
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &db);
    res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
        char   *p = db.ptr;
        char   *q;
        size_t  i = 0;

        while (i < size && (q = strchr(p, '\n')) != NULL) {
            *q = 0;
            ((unsigned char *) buf)[i++] = (unsigned char) atoi(p);
            p = q + 1;
        }
    }
    free(db.ptr);
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

static uint32_t
randombytes_randomorg(void)
{
    uint32_t r;

    randombytes_randomorg_buf(&r, sizeof r);

    return r;
}
static const char *
randombytes_randomorg_implementation_name(void)
{
    return "random.org";
}

struct randombytes_implementation randombytes_randomorg_implementation = {
    SODIUM_C99(.implementation_name =) randombytes_randomorg_implementation_name,
    SODIUM_C99(.random =) randombytes_randomorg,
    SODIUM_C99(.stir =) NULL,
    SODIUM_C99(.uniform =) NULL,
    SODIUM_C99(.buf =) randombytes_randomorg_buf,
    SODIUM_C99(.close =) NULL
};
