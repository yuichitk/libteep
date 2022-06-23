/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "teep.h"
#include "teep_http_client.h"

static size_t write_callback(void *recv_buffer_ptr,
                             size_t size,
                             size_t nitems,
                             void *user_ptr) {
    UsefulBuf *recv_buffer = (UsefulBuf *)user_ptr;
    printf("\nwrite_callback : start\n");
    size_t recv_size = size * nitems;

    if (recv_size > recv_buffer->len) {
        recv_size = recv_buffer->len;
    }
    memcpy(recv_buffer->ptr, recv_buffer_ptr, recv_size);
    recv_buffer->len = recv_size;
    return recv_size;
}

teep_err_t teep_send_http_post(const char *url,
                               UsefulBufC send_buffer,
                               UsefulBuf *recv_buffer) {
    teep_err_t          result = TEEP_SUCCESS;
    CURL                *curl = NULL;
    CURLcode            curl_result;
    struct curl_slist   *curl_slist = NULL;

    // Set parameter.
    printf("teep_send_post_request : Send HTTP Post request.\n");
    curl = curl_easy_init();
    if (curl == NULL) {
        printf("teep_send_post_request : curl_easy_init : Fail.\n");
        return TEEP_ERR_UNEXPECTED_ERROR;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_slist = curl_slist_append(curl_slist, "Accept: application/teep+cbor");
    curl_slist = curl_slist_append(curl_slist, "User-Agent: Foo/1.0");
    curl_slist = curl_slist_append(curl_slist, "Content-Type: application/teep+cbor");
    if (UsefulBuf_IsNULLOrEmptyC(send_buffer)) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 0L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
    }
    else {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, send_buffer.len);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, send_buffer.ptr);
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, recv_buffer);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    // Send request.
    curl_result = curl_easy_perform(curl);
    if (curl_result != CURLE_OK) {
        printf("teep_send_post_request : curl_easy_perform : Fail.\n");
        result = TEEP_ERR_ON_HTTP_POST;
        goto out;
    }

    // Get status code.
    int64_t response_code = -1;
    curl_result = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    if (curl_result != CURLE_OK || response_code < 0 || response_code != 200) {
        result = TEEP_ERR_ABORT;
        goto out;
    }

out:
    curl_easy_cleanup(curl);
    return result;
}
