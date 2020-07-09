/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include "teep_common.h"
#include "teep_cose.h"
#include "teep_http_client.h"
#include "teep_message_data.h"
#include "teep_message_print.h"
#include "teep_examples_common.h"

#define TAM_URL                         "http://localhost:8080/tam"
#define MAX_RECIEVE_BUFFER_SIZE         1024
#define TEEP_QUERY_RESPONSE_CBOR_FILE   "./testfiles/query_response_cose.cbor"
#define TEEP_SUCCESS_CBOR_FILE          "./testfiles/teep_success_cose.cbor"

int main(int argc, const char * argv[]) {
    // Send TEEP/HTTP POST request.
    printf("\nmain : Send TEEP/HTTP POST request.\n");
    teep_buf_t          recv_buffer;
    static uint8_t      buffer_impl[MAX_RECIEVE_BUFFER_SIZE];
    int32_t             result;
    recv_buffer.ptr = buffer_impl;
    recv_buffer.len = MAX_RECIEVE_BUFFER_SIZE;
    result = teep_send_http_post(TAM_URL, NULL, &recv_buffer);
    if (result) {
        printf("main : Fail to send TEEP/HTTP POST request.\n");
        return EXIT_FAILURE;
    }

    // Verify and print QueryRequest cose.
    UsefulBufC query_request_cose = {recv_buffer.ptr, recv_buffer.len};
    printf("\nmain : Print query_request_cose\n");
    print_hex(query_request_cose.ptr, query_request_cose.len);
    printf("\n");
    UsefulBufC query_request_payload;
    int32_t result_cose;
    result_cose = verify_cose_sign1(&query_request_cose, TAM_PUBLIC_KEY_PRIME256V1, &query_request_payload);
    if (result_cose) {
        printf("main : Fail to verify QueryRequest cose.\n");
        return EXIT_FAILURE;
    }
    printf("\nmain : Verify payload\n");
    print_hex(query_request_payload.ptr, query_request_payload.len);
    printf("\n");
    printf("\nmain : Print payload\n");
    print_teep_message(query_request_payload.ptr, query_request_payload.len);

    // Read QueryResponse cbor file.
    printf("main : Read QueryResponse cbor file.\n");
    size_t file_length;
    uint8_t *query_response_file_bytes = teep_read_file(TEEP_QUERY_RESPONSE_CBOR_FILE, &file_length);
    if (!file_length) {
        printf("main : Can't read QueryResponse cbor file.\n");
        return EXIT_FAILURE;
    }
    print_hex(query_response_file_bytes, file_length);
    printf("\n");

    // Send TEEP/HTTP QueryResponse.
    printf("\nmain : Send TEEP/HTTP QueryResponse.\n");
    teep_buf_t send_query_response_buffer = {file_length, query_response_file_bytes};
    result = teep_send_http_post(TAM_URL, &send_query_response_buffer, &recv_buffer);
    if (result) {
        printf("main : Fail to send TEEP/HTTP QueryResponse.\n");
        return EXIT_FAILURE;
    }

    // Verify and print TrustedAppInstall cose.
    UsefulBufC trusted_app_install_cose = {recv_buffer.ptr, recv_buffer.len};
    UsefulBufC trusted_app_install_payload;
    result_cose = verify_cose_sign1(&trusted_app_install_cose, TAM_PUBLIC_KEY_PRIME256V1, &trusted_app_install_payload);
    if (result_cose) {
        printf("main : Fail to verify TrustedAppInstall cose.\n");
        return EXIT_FAILURE;
    }
    printf("\nmain : Verify payload\n");
    print_hex(trusted_app_install_payload.ptr, trusted_app_install_payload.len);
    printf("\n");
    printf("\nmain : Print payload\n");
    print_teep_message(trusted_app_install_payload.ptr, trusted_app_install_payload.len);

    // Read Success cbor file.
    printf("main : Read Success cbor file.\n");
    uint8_t *success_file_bytes = teep_read_file(TEEP_SUCCESS_CBOR_FILE, &file_length);
    if (!file_length) {
        printf("main : Can't read Success cbor file.\n");
        return EXIT_FAILURE;
    }
    print_hex(success_file_bytes, file_length);
    printf("\n");

    // Send TEEP/HTTP Success.
    printf("\nmain : Send TEEP/HTTP Success.\n");
    teep_buf_t send_success_buffer = {file_length, success_file_bytes};
    result = teep_send_http_post(TAM_URL, &send_success_buffer, &recv_buffer);
    if (result) {
        printf("main : Fail to send TEEP/HTTP Success.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
