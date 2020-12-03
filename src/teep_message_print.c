/*
 * Copyright (c) 2020 SECOM CO., LTD. All Rights reserved.
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "teep_common.h"
#include "teep_message_data.h"
#include "teep_message_print.h"

void print_teep_query_request(const teep_query_request_t *query_request) {
    printf("  QueryRequest :\n");
    printf("    type : %u\n", query_request->type);
    printf("    options :\n");
    if (query_request->data_item_requested & TEEP_DATA_ITEM_ATTESTATION && query_request->token != TEEP_TOKEN_INVALID) {
        printf("      token : %u\n", query_request->token);
    }
    if (TEEP_ARRAY_IS_NOT_NULL(query_request->supported_cipher_suites)) {
        printf("      supported-cipher-suites : [");
        for (size_t i = 0; i < query_request->supported_cipher_suites.len; i++) {
            printf("%lu, ", query_request->supported_cipher_suites.items[i]);
        }
        printf("]\n");
    }
    if (TEEP_BUF_IS_NOT_NULL(query_request->challenge)) {
        printf("      challenge : ");
        print_hex(query_request->challenge.ptr, query_request->challenge.len);
        printf("\n");
    }
    if (TEEP_ARRAY_IS_NOT_NULL(query_request->versions)) {
        printf("      versions : [ ");
        for (size_t i = 0; i < query_request->versions.len; i++) {
            printf("%u, ", query_request->versions.items[i]);
        }
        printf("]\n");
    }
    if (TEEP_BUF_IS_NOT_NULL(query_request->ocsp_data)) {
        printf("      ocsp-data : ");
        print_hex(query_request->ocsp_data.ptr, query_request->ocsp_data.len);
        printf("\n");
    }
    printf("    data-item-requested : %u\n", query_request->data_item_requested);
}

void print_teep_query_response(const teep_query_response_t *query_response) {
    printf("  QueryResponse :\n");
    printf("    type : %u\n", query_response->type);
    printf("    options :\n");
    if (query_response->token != TEEP_TOKEN_INVALID) {
        printf("      token : %u\n", query_response->token);
    }
    if (query_response->selected_cipher_suite != TEEP_SUITE_INVALID) {
        printf("      selected-cipher-suite : %lu\n", query_response->selected_cipher_suite);
    }
    if (query_response->selected_version != TEEP_VERSION_INVALID) {
        printf("      selected-version : %u\n", query_response->selected_version);
    }
    if (TEEP_BUF_IS_NOT_NULL(query_response->evidence_format)) {
        printf("      evidence-format : ");
        if (query_response->evidence_format.len <= MAX_PRINT_TEXT_COUNT) {
            print_text(query_response->evidence_format.ptr, query_response->evidence_format.len);
            printf("\n");
        }
        else {
            print_text(query_response->evidence_format.ptr, MAX_PRINT_TEXT_COUNT);
            printf("..\n");
        }
    }
    if (TEEP_BUF_IS_NOT_NULL(query_response->evidence)) {
        printf("      evidence : ");
        print_hex(query_response->evidence.ptr, query_response->evidence.len);
        printf("\n");
    }
    if (TEEP_ARRAY_IS_NOT_NULL(query_response->tc_list)) {
        printf("      tc-list : [\n");
        for (size_t i = 0; i < query_response->tc_list.len; i++) {
            printf("        No.%ld = [ ", i);
            for (size_t j = 0; j < query_response->tc_list.items[i].component_id.len; j++) {
                if (query_response->tc_list.items[i].component_id.items[j].len <= MAX_PRINT_BYTE_COUNT) {
                    print_hex(query_response->tc_list.items[i].component_id.items[j].ptr, query_response->tc_list.items[i].component_id.items[j].len);
                }
                else {
                    print_hex(query_response->tc_list.items[i].component_id.items[j].ptr, MAX_PRINT_BYTE_COUNT);
                    printf("..");
                }
                if (query_response->tc_list.items[i].tc_manifest_sequence_number != TEEP_SUIT_MANIFEST_SEQUENCE_NUMBER_INVALID) {
                    printf("(seq=%lu)", query_response->tc_list.items[i].tc_manifest_sequence_number);
                }
                printf(", ");
            }
            printf("]\n");
        }
        printf("      ]\n");
    }
    if (TEEP_ARRAY_IS_NOT_NULL(query_response->ext_list)) {
        printf("      ext-list : [");
        for (size_t i = 0; i < query_response->ext_list.len; i++) {
            printf("%lu ", query_response->ext_list.items[i]);
        }
        printf("]\n");
    }
}

void print_teep_update(const teep_update_t *teep_update) {
    printf("  Update :\n");
    printf("    type : %u\n", teep_update->type);
    printf("    options :\n");
    if (teep_update->token != TEEP_TOKEN_INVALID) {
        printf("      token : %u\n", teep_update->token);
    }
    if (TEEP_ARRAY_IS_NOT_NULL(teep_update->manifest_list)) {
        printf("      manifest-list : [");
        for (size_t i = 0; i < teep_update->manifest_list.len; i++) {
            printf("No.%ld = ", i);
            if (teep_update->manifest_list.items[i].len <= MAX_PRINT_BYTE_COUNT) {
                print_hex(teep_update->manifest_list.items[i].ptr, teep_update->manifest_list.items[i].len);
                printf(", ");
            }
            else {
                print_hex(teep_update->manifest_list.items[i].ptr, MAX_PRINT_BYTE_COUNT);
                printf(".., ");
            }
        }
        printf("]\n");
    }
}

void print_teep_error(const teep_error_t *error) {
    printf("  Error :\n");
    printf("    type : %u\n", error->type);
    printf("    options :\n");
    if (error->token != TEEP_TOKEN_INVALID) {
        printf("      token : %u\n", error->token);
    }
    if (TEEP_BUF_IS_NOT_NULL(error->err_msg)) {
        printf("      err-msg : ");
        if (error->err_msg.len <= MAX_PRINT_TEXT_COUNT) {
            print_text(error->err_msg.ptr, error->err_msg.len);
            printf("\n");
        }
        else {
            print_text(error->err_msg.ptr, MAX_PRINT_TEXT_COUNT);
            printf("..\n");
        }
    }
    if (TEEP_ARRAY_IS_NOT_NULL(error->supported_cipher_suites)) {
        printf("      supported-cipher-suites : ");
        for (size_t i = 0; i < error->supported_cipher_suites.len; i++) {
            printf("%u ", error->supported_cipher_suites.items[i]);
        }
        printf("\n");
    }
    if (TEEP_ARRAY_IS_NOT_NULL(error->versions)) {
        printf("      versions : ");
        for (size_t i = 0; i < error->versions.len; i++) {
            printf("%u ", error->versions.items[i]);
        }
        printf("\n");
    }
    printf("    err-code : %u\n", error->err_code);
    printf("\n");
}

void print_teep_success(const teep_success_t *success) {
    printf("  Success :\n");
    printf("    type : %u\n", success->type);
    printf("    options :\n");
    if (success->token != TEEP_TOKEN_INVALID) {
        printf("      token : %u\n", success->token);
    }
    if (TEEP_BUF_IS_NOT_NULL(success->msg)) {
        printf("    msg : ");
        if (success->msg.len <= MAX_PRINT_TEXT_COUNT) {
            print_text(success->msg.ptr, success->msg.len);
            printf("\n");
        }
        else {
            print_text(success->msg.ptr, MAX_PRINT_TEXT_COUNT);
            printf("..\n");
        }
    }
}

void print_teep_message(const uint8_t *message, const size_t message_len) {
    // decode cbor : get message type.
    QCBORDecodeContext  decode_context;
    QCBORDecode_Init(&decode_context,
                     (UsefulBufC){message, message_len},
                     QCBOR_DECODE_MODE_NORMAL);
    uint64_t message_type = get_teep_message_type(&decode_context);
    QCBORDecode_Finish(&decode_context);

    // decode cbor : get message.
    QCBORDecode_Init(&decode_context,
                     (UsefulBufC){message, message_len},
                     QCBOR_DECODE_MODE_NORMAL);
    union {
        teep_query_request_t query_request;
        teep_query_response_t query_response;
        teep_update_t teep_update;
        teep_success_t teep_success;
        teep_error_t teep_error;
    } d;
    switch (message_type) {
        case TEEP_TYPE_QUERY_REQUEST:
            set_teep_query_request(&decode_context, &d.query_request);
            print_teep_query_request(&d.query_request);
            break;
        case TEEP_TYPE_QUERY_RESPONSE:
            set_teep_query_response(&decode_context, &d.query_response);
            print_teep_query_response(&d.query_response);
            break;
        case TEEP_TYPE_UPDATE:
            set_teep_update(&decode_context, &d.teep_update);
            print_teep_update(&d.teep_update);
            break;
        case TEEP_TYPE_TEEP_SUCCESS:
            set_teep_success(&decode_context, &d.teep_success);
            print_teep_success(&d.teep_success);
            break;
        case TEEP_TYPE_TEEP_ERROR:
            set_teep_error(&decode_context, &d.teep_error);
            print_teep_error(&d.teep_error);
            break;
        default:
            print_debug_string("print_teep_message : Undefined value.\n");
            break;
    }
    QCBORDecode_Finish(&decode_context);
}
