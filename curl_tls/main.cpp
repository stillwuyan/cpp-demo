/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2020, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/
/* <DESC>
 * Simple HTTPS GET
 * </DESC>
 */
#include <stdio.h>
#include <curl/curl.h>
#include <chrono>
#include <thread>

const char *pEngine = NULL;

bool setup_ssl(CURL* curl)
{
    if(curl_easy_setopt(curl, CURLOPT_SSLENGINE, pEngine) != CURLE_OK) {
        /* load the crypto engine */
        printf("can't set crypto engine\n");
        return false;
    }
    if(curl_easy_setopt(curl, CURLOPT_SSLENGINE_DEFAULT, 1L) != CURLE_OK) {
        /* set the crypto engine as default */
        /* only needed for the first time you load
         * a engine in a curl object... */
        printf("can't set crypto engine as default\n");
        return false;
    }

    // client certification and key
    curl_easy_setopt(curl, CURLOPT_SSLCERTTYPE, "PEM");
    curl_easy_setopt(curl, CURLOPT_SSLCERT, "/srv/data/Joynext/xu_q2/workspace/demo/cpp-demo/curl_tls/cert/client.crt");
    curl_easy_setopt(curl, CURLOPT_SSLKEYTYPE, "PEM");
    curl_easy_setopt(curl, CURLOPT_SSLKEY, "/srv/data/Joynext/xu_q2/workspace/demo/cpp-demo/curl_tls/cert/client.key");
    // root certification
    curl_easy_setopt(curl, CURLOPT_CAINFO, "/srv/data/Joynext/xu_q2/workspace/demo/cpp-demo/curl_tls/cert/server.crt");

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 1);
    return true;
}

void finish(CURL* curl)
{
    if (curl) {
        curl_easy_cleanup(curl);
    }
}

void get(CURL* curl, const char* url)
{
    /* Perform the request, res will get the return code */
    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK) {
        printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
    }
}

int main(void)
{
    CURL *curl = NULL;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (setup_ssl(curl)) {

        for (int i = 0; i < 10; i++) {
            get(curl, "https://localhost:9530");
            std::this_thread::sleep_for(std::chrono::seconds(1));
            get(curl, "https://localhost:9530/another.html");
        }

        finish(curl);
    }
    curl_global_cleanup();
    return 0;
}
