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
    UsefulBufC query_request_payload;
    int32_t result_cose;

    result_cose = verify_cose_sign1(&query_request_cose, key_buf, &query_request_payload);
    if (result_cose) {
        printf("main : Fail to verify QueryRequest cose.\n");
#ifdef ALLOW_CBOR_WITHOUT_SIGN1
        if (TEEP_CBOR_WITHOUT_SIGN1 == result_cose) {
            goto query_response;
        }
#endif
        return EXIT_FAILURE;
    }
    printf("\nmain : Verify payload\n");
    teep_print_hex(query_request_payload.ptr, query_request_payload.len);
    printf("\n");
    printf("\nmain : Print payload\n");
    print_teep_message(query_request_payload.ptr, query_request_payload.len);

#ifdef ALLOW_CBOR_WITHOUT_SIGN1
query_response:
#endif
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
    UsefulBufC update_payload;
    result_cose = verify_cose_sign1(&update_cose, key_buf, &update_payload);
    if (result_cose) {
        printf("main : Fail to verify Update cose.\n");
#ifdef ALLOW_CBOR_WITHOUT_SIGN1
        if (TEEP_CBOR_WITHOUT_SIGN1 == result_cose) {
            goto teep_success;
        }
#endif
        return EXIT_FAILURE;
    }
    printf("\nmain : Verify payload\n");
    teep_print_hex(update_payload.ptr, update_payload.len);
    printf("\n");
    printf("\nmain : Print payload\n");
    print_teep_message(update_payload.ptr, update_payload.len);

#ifdef ALLOW_CBOR_WITHOUT_SIGN1
teep_success:
#endif
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
