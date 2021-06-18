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

const char DEFAULT_TAM_URL[] =          "http://localhost:8080/tam";
#define MAX_RECIEVE_BUFFER_SIZE         1024
#define MAX_FILE_BUFFER_SIZE            512
#ifdef SEND_CBOR_WITHOUT_SIGN1
  #define TEEP_QUERY_RESPONSE_CBOR_FILE   "./testfiles/query_response.cbor"
  #define TEEP_SUCCESS_CBOR_FILE          "./testfiles/teep_success.cbor"
#else
  #define TEEP_QUERY_RESPONSE_CBOR_FILE   "./testfiles/query_response_cose.cbor"
  #define TEEP_SUCCESS_CBOR_FILE          "./testfiles/teep_success_cose.cbor"
#endif
#define TAM_PUBLIC_KEY_DER_FILE         "./testfiles/key/tam_prime256v1_pub.der"

int main(int argc, const char * argv[]) {
    const char *tam_url = DEFAULT_TAM_URL;
    if (argc > 1) {
        tam_url = argv[1];
    }
    // Read der file.
    printf("\nmain : Read DER file.\n");
    uint8_t der_buf[PRIME256V1_PUBLIC_KEY_DER_SIZE];
    size_t der_len = read_from_file(TAM_PUBLIC_KEY_DER_FILE, PRIME256V1_PUBLIC_KEY_DER_SIZE, der_buf);
    if (!der_len) {
        printf("main : Can't read DER file.\n");
        return EXIT_FAILURE;
    }
    teep_print_hex(der_buf, der_len);
    printf("\n");

    // Read key from der file.
    // This code is only available for openssl prime256v1.
    printf("\nmain : Read key from DER file.\n");
    char key_buf[PRIME256V1_PUBLIC_KEY_CHAR_SIZE];
    read_prime256v1_public_key(der_buf, key_buf);
    printf("%s\n", key_buf);

    // Send TEEP/HTTP POST request.
    printf("\nmain : Send TEEP/HTTP POST request.\n");
    teep_buf_t          recv_buffer;
    static uint8_t      buffer_impl[MAX_RECIEVE_BUFFER_SIZE];
    int32_t             result;
    recv_buffer.ptr = buffer_impl;
    recv_buffer.len = MAX_RECIEVE_BUFFER_SIZE;
    result = teep_send_http_post(tam_url, NULL, &recv_buffer);
    if (result) {
        printf("main : Fail to send TEEP/HTTP POST request.\n");
        return EXIT_FAILURE;
    }

    // Verify and print QueryRequest cose.
    UsefulBufC query_request_cose = {recv_buffer.ptr, recv_buffer.len};
    printf("\nmain : Print query_request_cose\n");
    teep_print_hex(query_request_cose.ptr, query_request_cose.len);
    printf("\n");

    teep_message_t query_request;
    result = teep_set_message_from_bytes(query_request_cose.ptr, query_request_cose.len, &query_request);
    print_teep_message(&query_request, 2, key_buf);

    // Read QueryResponse cbor file.
    printf("main : Read QueryResponse cbor file.\n");
    uint8_t query_response_file_bytes[MAX_FILE_BUFFER_SIZE];
    size_t file_length = read_from_file(TEEP_QUERY_RESPONSE_CBOR_FILE, MAX_FILE_BUFFER_SIZE, query_response_file_bytes);
    if (!file_length) {
        printf("main : Can't read CBOR file.\n");
        return EXIT_FAILURE;
    }
    teep_print_hex(query_response_file_bytes, file_length);
    printf("\n");

    // Send TEEP/HTTP QueryResponse.
    recv_buffer.len = MAX_RECIEVE_BUFFER_SIZE;
    printf("\nmain : Send TEEP/HTTP QueryResponse.\n");
    teep_buf_t send_query_response_buffer = {file_length, query_response_file_bytes};
    result = teep_send_http_post(tam_url, &send_query_response_buffer, &recv_buffer);
    if (result) {
        printf("main : Fail to send TEEP/HTTP QueryResponse.\n");
        return EXIT_FAILURE;
    }

    // Verify and print Update cose.
    UsefulBufC update_cose = {recv_buffer.ptr, recv_buffer.len};
    teep_message_t update;
    result = teep_set_message_from_bytes(update_cose.ptr, update_cose.len, &update);
    if (result) {
        printf("main : Fail to decode QueryResponse %d\n", result);
        return EXIT_FAILURE;
    }
    print_teep_message(&update, 2, key_buf);

    // Read Success cbor file.
    printf("main : Read Success cbor file.\n");
    uint8_t success_file_bytes[MAX_FILE_BUFFER_SIZE];
    file_length = read_from_file(TEEP_SUCCESS_CBOR_FILE, MAX_FILE_BUFFER_SIZE, success_file_bytes);
    if (!file_length) {
        printf("main : Can't read Success cbor file.\n");
        return EXIT_FAILURE;
    }
    teep_print_hex(success_file_bytes, file_length);
    printf("\n");

    // Send TEEP/HTTP Success.
    printf("\nmain : Send TEEP/HTTP Success.\n");
    teep_buf_t send_success_buffer = {file_length, success_file_bytes};
    result = teep_send_http_post(tam_url, &send_success_buffer, &recv_buffer);
    if (result) {
        printf("main : Fail to send TEEP/HTTP Success.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
