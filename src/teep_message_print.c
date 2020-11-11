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
    printf("    token : %u\n", query_request->token);
    printf("    options :\n");
    if (query_request->supported_suites.len > 0) {
        printf("      supported-suites : [");
        for (size_t i = 0; i < query_request->supported_suites.len; i++) {
            printf("%u, ", query_request->supported_suites.items[i]);
        }
        printf("]\n");
    }
    if (query_request->suite != TEEP_SUITE_INVALID) {
        printf("      suite : %u\n", query_request->suite);
    }
    if (query_request->nonce.len > 0) {
        printf("      nonce : ");
        print_hex(query_request->nonce.ptr, query_request->nonce.len);
        printf("\n");
    }
    if (query_request->version.len > 0) {
        printf("      version : [ ");
        for (size_t i = 0; i < query_request->version.len; i++) {
            printf("%u, ", query_request->version.items[i]);
        }
        printf("]\n");
    }
    if (query_request->ocsp_data.len > 0) {
        printf("      oscp-data : ");
        print_hex(query_request->ocsp_data.ptr, query_request->ocsp_data.len);
        printf("\n");
    }
    printf("    data-item-requested : %u\n", query_request->data_item_requested);
}

void print_teep_query_response(const teep_query_response_t *query_response) {
    printf("  QueryResponse :\n");
    printf("    type : %u\n", query_response->type);
    printf("    token : %u\n", query_response->token);
    printf("    options :\n");
    if (query_response->suite != TEEP_SUITE_INVALID) {
        printf("      suite : %u\n", query_response->suite);
    }
    if (query_response->version != TEEP_INVALID_VERSION) {
        printf("      version : %u\n", query_response->version);
    }
    if (query_response->eat.len > 0) {
        printf("      eat : ");
        print_hex(query_response->eat.ptr, query_response->eat.len);
        printf("\n");
    }
    if (query_response->ta_list.len > 0) {
        printf("      ta-list : ");
        for (size_t i = 0; i < query_response->ta_list.len; i++) {
            printf("No.%ld = ", i);
            if (query_response->ta_list.items[i].len <= MAX_PRINT_BYTE_COUNT) {
                print_hex(query_response->ta_list.items[i].ptr, query_response->ta_list.items[i].len);
                printf(", ");
            }
            else {
                print_hex(query_response->ta_list.items[i].ptr, MAX_PRINT_BYTE_COUNT);
                printf(".., ");
            }
        }
        printf("\n");
    }
    if (query_response->ext_info.len > 0) {
        printf("      ext-list : ");
        for (size_t i = 0; i < query_response->ext_info.len; i++) {
            printf("%u ", query_response->ext_info.items[i]);
        }
        printf("\n");
    }
}

void print_teep_install(const teep_install_t *app_install) {
    printf("  TrustedAppInstall :\n");
    printf("    type : %u\n", app_install->type);
    printf("    token : %u\n", app_install->token);
    printf("    options :\n");
    if (app_install->manifest_list.len > 0) {
        printf("      manifest-list : ");
        for (size_t i = 0; i < app_install->manifest_list.len; i++) {
            printf("No.%ld = ", i);
            if (app_install->manifest_list.items[i].len <= MAX_PRINT_BYTE_COUNT) {
                print_hex(app_install->manifest_list.items[i].ptr, app_install->manifest_list.items[i].len);
                printf(", ");
            }
            else {
                print_hex(app_install->manifest_list.items[i].ptr, MAX_PRINT_BYTE_COUNT);
                printf(".., ");
            }
        }
        printf("\n");
    }
}

void print_teep_delete(const teep_delete_t *app_delete) {
    printf("  TrustedAppDelete :\n");
    printf("    type : %u\n", app_delete->type);
    printf("    token : %u\n", app_delete->token);
    printf("    options :\n");
    if (app_delete->ta_list.len > 0) {
        printf("      ta-list : ");
        for (size_t i = 0; i < app_delete->ta_list.len; i++) {
            printf("No.%ld = ", i);
            if (app_delete->ta_list.items[i].len <= MAX_PRINT_BYTE_COUNT) {
                print_hex(app_delete->ta_list.items[i].ptr, app_delete->ta_list.items[i].len);
                printf(", ");
            }
            else {
                print_hex(app_delete->ta_list.items[i].ptr, MAX_PRINT_BYTE_COUNT);
                printf(".., ");
            }
            printf(".., ");
        }
        printf("\n");
    }
}

void print_teep_error(const teep_error_t *error) {
    printf("  Error :\n");
    printf("    type : %u\n", error->type);
    printf("    token : %u\n", error->token);
    printf("    options :\n");
    if (error->err_msg.len > 0) {
        printf("      err-msg : ");
        print_hex(error->err_msg.ptr, error->err_msg.len);
        printf("\n");
    }
    if (error->suite.len > 0) {
        printf("      suite : ");
        for (size_t i = 0; i < error->suite.len; i++) {
            printf("%u ", error->suite.items[i]);
        }
        printf("\n");
    }
    if (error->version.len > 0) {
        printf("      version : ");
        for (size_t i = 0; i < error->version.len; i++) {
            printf("%u ", error->version.items[i]);
        }
        printf("\n");
    }
    printf("    err-code : %u\n", error->err_code);
    printf("\n");
}

void print_teep_success(const teep_success_t *success) {
    printf("  Success :\n");
    printf("    type : %u\n", success->type);
    printf("    token : %u\n", success->token);
    printf("    options :\n");
    if (success->msg.len > 0) {
        printf("    msg : ");
        print_hex(success->msg.ptr, success->msg.len);
        printf("\n");
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
    teep_query_request_t query_request;
    teep_query_response_t query_response;
    teep_install_t app_install;
    teep_delete_t app_delete;
    teep_success_t teep_success;
    teep_error_t teep_error;
    switch (message_type) {
        case TEEP_TYPE_QUERY_REQUEST:
            set_teep_query_request(&decode_context, &query_request);
            print_teep_query_request(&query_request);
            break;
        case TEEP_TYPE_QUERY_RESPONSE:
            set_teep_query_response(&decode_context, &query_response);
            print_teep_query_response(&query_response);
            break;
        case TEEP_TYPE_INSTALL:
            set_teep_install(&decode_context, &app_install);
            print_teep_install(&app_install);
            break;
        case TEEP_TYPE_DELETE:
            set_teep_delete(&decode_context, &app_delete);
            print_teep_delete(&app_delete);
            break;
        case TEEP_TYPE_TEEP_SUCCESS:
            set_teep_success(&decode_context, &teep_success);
            print_teep_success(&teep_success);
            break;
        case TEEP_TYPE_TEEP_ERROR:
            set_teep_error(&decode_context, &teep_error);
            print_teep_error(&teep_error);
            break;
        default:
            print_debug_string("print_teep_message : Undefined value.\n");
            break;
    }
    QCBORDecode_Finish(&decode_context);
}
