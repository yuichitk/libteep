/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include "teep_cose.h"
#include "teep_http_client.h"
#include "teep_message_data.h"
#include "teep_message_print.h"
#include "teep_examples_common.h"

#include "teep_agent_es256_private_key.h"
#include "teep_agent_es256_public_key.h"
const char *teep_agent_private_key = teep_agent_es256_private_key;
const char *teep_agent_public_key = teep_agent_es256_public_key;
#include "tam_es256_public_key.h"
const char *tam_public_key = tam_es256_public_key;

const char DEFAULT_TAM_URL[] =          "http://localhost:8080/tam";
#define MAX_RECEIVE_BUFFER_SIZE         1024
#define MAX_SEND_BUFFER_SIZE            1024
#define MAX_FILE_BUFFER_SIZE            512

#define SUPPORTED_VERSION               0
const teep_ciphersuite_t supported_ciphersuites[] = {
    {.sign = TEEP_COSE_SIGN_ES256, .encrypt = TEEP_COSE_ENCRYPT_NONE, .mac = TEEP_COSE_MAC_NONE}
};

teep_err_t get_update(const char *tam_url,
                      const teep_message_t *query_response,
                      UsefulBuf cbor_buf,
                      const struct t_cose_key *verifying_key,
                      teep_message_t *update) {
    return TEEP_SUCCESS;
}

teep_err_t create_query_response(const teep_query_request_t *query_request,
                                 teep_query_response_t *query_response) {
    size_t i;

    int32_t version = -1;
    teep_ciphersuite_t ciphersuite = TEEP_CIPHERSUITE_INVALID;

    if (query_request->contains & TEEP_MESSAGE_CONTAINS_VERSION) {
        for (i = 0; i < query_request->versions.len; i++) {
            if (query_request->versions.items[i] == SUPPORTED_VERSION) {
                /* supported version is found */
                version = SUPPORTED_VERSION;
                break;
            }
        }
    }
    else {
        /* means version=0 is supported */
        version = 0;
    }
    if (version != SUPPORTED_VERSION) {
        return TEEP_ERR_NO_SUPPORTED_VERSION;
    }

    if (!(query_request->contains & TEEP_MESSAGE_CONTAINS_SUPPORTED_CIPHER_SUITES)) {
        /* TODO */
        ciphersuite = supported_ciphersuites[0];
    }
    for (i = 0; i < query_request->supported_cipher_suites.len; i++) {
        for (size_t j = 0; j < sizeof(supported_ciphersuites) / sizeof(teep_ciphersuite_t); j++) {
            if (teep_ciphersuite_is_same(query_request->supported_cipher_suites.items[i], supported_ciphersuites[j])) {
                /* supported ciphersuite is found */
                ciphersuite = supported_ciphersuites[j];
            }
        }
    }
    if (teep_ciphersuite_is_same(ciphersuite, TEEP_CIPHERSUITE_INVALID)) {
        return TEEP_ERR_NO_SUPPORTED_CIPHERSUITE;
    }

    memset(query_response, 0, sizeof(teep_query_response_t));
    query_response->type = TEEP_TYPE_QUERY_RESPONSE;
    query_response->contains = TEEP_MESSAGE_CONTAINS_TOKEN |
                               TEEP_MESSAGE_CONTAINS_VERSION |
                               TEEP_MESSAGE_CONTAINS_SELECTED_CIPHER_SUITE;
    query_response->token = query_request->token;
    query_response->selected_version = version;
    query_response->selected_cipher_suite = ciphersuite;

    return TEEP_SUCCESS;
}

teep_err_t get_query_request(const char *tam_url,
                             UsefulBuf cbor_buf,
                             const struct t_cose_key *verifying_key,
                             teep_query_request_t *query_request) {
    teep_err_t result;

    // Send TEEP/HTTP POST request.
    printf("\nmain : Send TEEP/HTTP POST request.\n");
    result = teep_send_http_post(tam_url, NULLUsefulBufC, &cbor_buf);
    if (result) {
        printf("main : Failed to send TEEP/HTTP POST request.\n");
        return TEEP_ERR_ON_HTTP_POST;
    }

    // Verify and print QueryRequest cose.
    UsefulBufC payload;
    result = verify_cose_sign1(UsefulBuf_Const(cbor_buf), verifying_key, &payload);
    if (result != TEEP_SUCCESS) {
        printf("\nmain : Failed to verify QueryRequest. %d\n", result);
        return TEEP_ERR_SIGNING_FAILED;
    }

    result = teep_set_message_from_bytes(payload.ptr, payload.len, (teep_message_t *)query_request);
    return result;
}

int main(int argc, const char * argv[]) {
    teep_err_t          result;
    const char *tam_url = DEFAULT_TAM_URL;
    if (argc > 1) {
        tam_url = argv[1];
    }

    struct t_cose_key t_cose_signing_key;
    result = create_key_pair(NID_X9_62_prime256v1, teep_agent_es256_private_key, teep_agent_es256_public_key, &t_cose_signing_key);
    if (result != TEEP_SUCCESS) {
        printf("main : Failed to create t_cose key pair. (%d)\n", result);
        return EXIT_FAILURE;
    }

    struct t_cose_key t_cose_verifying_key;
    result = create_public_key(NID_X9_62_prime256v1, tam_es256_public_key, &t_cose_verifying_key);
    if (result != TEEP_SUCCESS) {
        printf("main : Failed to parse t_cose public key. (%d)\n", result);
        return EXIT_FAILURE;
    }

    UsefulBuf_MAKE_STACK_UB(cbor_recv_buf, MAX_RECEIVE_BUFFER_SIZE);
    printf("cbor_buf = %p, %ld\n", cbor_recv_buf.ptr, cbor_recv_buf.len);
    teep_query_request_t query_request;
    result = get_query_request(tam_url, cbor_recv_buf, &t_cose_verifying_key, &query_request);
    if (result != TEEP_SUCCESS) {
        printf("main : Failed to parse QueryRequest. (%d)\n", result);
        return EXIT_FAILURE;
    }
    teep_print_message((teep_message_t *)&query_request, 2, NULL);

    teep_query_response_t query_response;
    result = create_query_response(&query_request, &query_response);
    if (result != TEEP_SUCCESS) {
        printf("main : Failed to create query_response message. (%d)\n", result);
        return EXIT_FAILURE;
    }
    UsefulBuf_MAKE_STACK_UB(cbor_send_buf, MAX_SEND_BUFFER_SIZE);
    result = teep_encode_message((teep_message_t *)&query_response, &cbor_send_buf.ptr, &cbor_send_buf.len);
    if (result != TEEP_SUCCESS) {
        printf("main : Failed to encode query_response message. (%d)\n", result);
        return EXIT_FAILURE;
    }
    UsefulBuf_MAKE_STACK_UB(cose_send_buf, MAX_SEND_BUFFER_SIZE);
    result = sign_cose_sign1(UsefulBuf_Const(cbor_send_buf), &t_cose_signing_key, T_COSE_ALGORITHM_ES256, &cose_send_buf);
    if (result != TEEP_SUCCESS) {
        printf("main : Failed to sign to query_response message. (%d)\n", result);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
