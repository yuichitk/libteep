/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdio.h>
#include <unistd.h>
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

teep_err_t create_error(teep_buf_t token,
                        uint64_t err_code_contains,
                        teep_message_t *message) {
    teep_error_t *error = (teep_error_t *)message;
    error->type = TEEP_TYPE_TEEP_ERROR;
    error->contains = err_code_contains | TEEP_MESSAGE_CONTAINS_TOKEN;

    if (err_code_contains & TEEP_ERR_CODE_UNSUPPORTED_MSG_VERSION) {
        error->versions.len = 1;
        error->versions.items[0] = SUPPORTED_VERSION;
        error->contains |= TEEP_MESSAGE_CONTAINS_VERSION;
        error->err_code = TEEP_ERR_CODE_UNSUPPORTED_MSG_VERSION;
    }
    else if (err_code_contains & TEEP_ERR_CODE_UNSUPPORTED_CIPHER_SUITES) {
        error->supported_cipher_suites.len = sizeof(supported_ciphersuites) / sizeof(teep_ciphersuite_t);
        for (size_t i = 0; i < error->supported_cipher_suites.len; i++) {
            error->supported_cipher_suites.items[i] = supported_ciphersuites[i];
        }
        error->contains |= TEEP_MESSAGE_CONTAINS_SUPPORTED_CIPHER_SUITES;
        error->err_code = TEEP_ERR_CODE_UNSUPPORTED_CIPHER_SUITES;
    }
    return TEEP_SUCCESS;
}

teep_err_t create_success_or_error(const teep_update_t *update,
                                   teep_message_t *message) {
    /* TODO: Process SUIT Manifest */

    // create SUCCESS message
    teep_success_t *success = (teep_success_t *)message;
    success->type = TEEP_TYPE_TEEP_SUCCESS;
    success->contains = TEEP_MESSAGE_CONTAINS_TOKEN;
    success->token = update->token;
    return TEEP_SUCCESS;
}

teep_err_t create_query_response_or_error(const teep_query_request_t *query_request,
                                          teep_message_t *message) {
    size_t i;
    uint64_t err_code_contains = 0;
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
        err_code_contains |= TEEP_ERR_CODE_UNSUPPORTED_MSG_VERSION;
        goto error;
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
        err_code_contains |= TEEP_ERR_CODE_UNSUPPORTED_CIPHER_SUITES;
        goto error;
    }

error: /* would be unneeded if the err-code becomes bit field */
    if (err_code_contains != 0) {
        return create_error(query_request->token, err_code_contains, message);
    }

    teep_query_response_t *query_response = (teep_query_response_t *)message;
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

teep_err_t get_teep_message(const char *tam_url,
                            UsefulBufC send_buf,
                            const struct t_cose_key *verifying_key,
                            UsefulBuf recv_buf,
                            teep_message_t *message) {
    teep_err_t result;

    // Send TEEP/HTTP POST request.
    printf("main : Send Buffer(len=%ld) on TEEP/HTTP POST request.\n", send_buf.len);
    result = teep_send_http_post(tam_url, send_buf, &recv_buf);
    if (result != TEEP_SUCCESS) {
        return result;
    }

    // Verify and print QueryRequest cose.
    UsefulBufC payload;
    result = verify_cose_sign1(UsefulBuf_Const(recv_buf), verifying_key, &payload);
    if (result != TEEP_SUCCESS) {
        printf("main : Failed to verify QueryRequest. %d\n", result);
        return result;
    }

    return teep_set_message_from_bytes(payload.ptr, payload.len, message);
}

int main(int argc, const char * argv[]) {
    teep_err_t          result;
    const char *tam_url = DEFAULT_TAM_URL;
    if (argc > 1) {
        tam_url = argv[1];
    }
    UsefulBuf_MAKE_STACK_UB(cbor_recv_buf, MAX_RECEIVE_BUFFER_SIZE);
    UsefulBuf_MAKE_STACK_UB(cbor_send_buf, MAX_SEND_BUFFER_SIZE);
    UsefulBuf_MAKE_STACK_UB(cose_send_buf, MAX_SEND_BUFFER_SIZE);

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

    teep_message_t send_message;
    teep_message_t recv_message;

    cose_send_buf.len = 0;
    while (1) {
        result = get_teep_message(tam_url, UsefulBuf_Const(cose_send_buf), &t_cose_verifying_key, cbor_recv_buf, &recv_message);
        if (result != TEEP_SUCCESS) {
            if (result == TEEP_ERR_ABORT) {
                /* just the TAM terminated the connection */
                result = TEEP_SUCCESS;
                printf("main : The TAM terminated the connection.\n");
                break;
            }
            printf("main : Failed to parse received message. (%d)\n", result);
            return EXIT_FAILURE;
        }
        teep_print_message(&recv_message, 2, NULL);

        cose_send_buf.len = MAX_SEND_BUFFER_SIZE;
        switch (recv_message.teep_message.type) {
        case TEEP_TYPE_QUERY_REQUEST:
            result = create_query_response_or_error((const teep_query_request_t *)&recv_message, &send_message);
            break;
        case TEEP_TYPE_UPDATE:
            result = create_success_or_error((const teep_update_t *)&recv_message, &send_message);
            break;
        case TEEP_TYPE_TEEP_ERROR:
            printf("main : TAM sent Error.\n");
            break;
        default:
            printf("main : Unexpected message type %d\n.", recv_message.teep_message.type);
            return EXIT_FAILURE;
        }
        if (result != TEEP_SUCCESS) {
            printf("main : Failed to create teep message. (%d)\n", result);
            return EXIT_FAILURE;
        }

        cbor_send_buf.len = MAX_SEND_BUFFER_SIZE;
        result = teep_encode_message(&send_message, &cbor_send_buf.ptr, &cbor_send_buf.len);
        if (result != TEEP_SUCCESS) {
            printf("main : Failed to encode query_response message. (%d)\n", result);
            return EXIT_FAILURE;
        }
        cose_send_buf.len = MAX_SEND_BUFFER_SIZE;
        result = sign_cose_sign1(UsefulBuf_Const(cbor_send_buf), &t_cose_signing_key, T_COSE_ALGORITHM_ES256, &cose_send_buf);
        if (result != TEEP_SUCCESS) {
            printf("main : Failed to sign to query_response message. (%d)\n", result);
            return EXIT_FAILURE;
        }
        sleep(1);
    }

    return EXIT_SUCCESS;
}
