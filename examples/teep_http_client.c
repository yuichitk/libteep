/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include "teep_http_client.h"

static size_t write_callback(void *recv_buffer_ptr,
                             size_t size,
                             size_t nitems,
                             void *user_ptr) {
    printf("\nwrite_callback : start\n");
    teep_buf_t *user_buffer = (teep_buf_t *)user_ptr;
    size_t recv_size = size * nitems;

    if (user_buffer->len < 0) {
        printf("\nwrite_callback : user_buffer->len < 0\n");
        return 0;
    }
    size_t copy_size = user_buffer->len;
    if (copy_size > recv_size) {
        copy_size = recv_size;
    }
    memcpy((void *)user_buffer->ptr, recv_buffer_ptr, copy_size);
    user_buffer->len = copy_size;
    return copy_size;
}

int32_t teep_send_http_post(char *url,
                            teep_buf_t *send_buffer,
                            teep_buf_t *recv_buffer) {
    CURL                *curl = NULL;
    CURLcode            result;
    struct curl_slist   *curl_slist = NULL;

    // Set parameter.
    printf("teep_send_post_request : Send HTTP Post request.\n");
    curl = curl_easy_init();
    if (curl == NULL) {
        printf("teep_send_post_request : curl_easy_init : Fail.\n");
        return TEEP_UNEXPECTED_ERROR;
    }
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_slist = curl_slist_append(curl_slist, "Accept: application/teep+cbor");
    curl_slist = curl_slist_append(curl_slist, "User-Agent: Foo/1.0");
    if (send_buffer == NULL) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 0L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
    }
    else {
        curl_slist = curl_slist_append(curl_slist, "Content-Type: application/teep+cbor");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, send_buffer->len);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, send_buffer->ptr);
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, recv_buffer);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    // Send request.
    result = curl_easy_perform(curl);
    if (result != CURLE_OK) {
        printf("teep_send_post_request : curl_easy_perform : Fail.\n");
        return TEEP_UNEXPECTED_ERROR;
    }
    curl_easy_cleanup(curl);

    return TEEP_SUCCESS;
}
