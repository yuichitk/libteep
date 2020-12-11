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

#ifdef PARSE_SUIT
#include "csuit/csuit.h"
#endif

void print_teep_query_request(const teep_query_request_t *query_request, uint32_t indent_space) {
    printf("%*sQueryRequest :\n", indent_space, "");
    printf("%*stype : %u\n", indent_space + 2, "", query_request->type);
    printf("%*soptions :\n", indent_space + 2, "");
    if (query_request->contains & TEEP_MESSAGE_CONTAINS_TOKEN) {
        printf("%*stoken : %u\n", indent_space + 4, "", query_request->token);
    }
    if (query_request->contains & TEEP_MESSAGE_CONTAINS_SUPPORTED_CIPHER_SUITES) {
        printf("%*ssupported-cipher-suites : [ ", indent_space + 4, "");
        for (size_t i = 0; i < query_request->supported_cipher_suites.len; i++) {
            printf("%lu, ", query_request->supported_cipher_suites.items[i]);
        }
        printf("]\n");
    }
    if (query_request->contains & TEEP_MESSAGE_CONTAINS_CHALLENGE) {
        printf("%*schallenge : ", indent_space + 4, "");
        teep_print_hex(query_request->challenge.ptr, query_request->challenge.len);
        printf("\n");
    }
    if (query_request->contains & TEEP_MESSAGE_CONTAINS_VERSION) {
        printf("%*sversions : [ ", indent_space + 4, "");
        for (size_t i = 0; i < query_request->versions.len; i++) {
            printf("%u, ", query_request->versions.items[i]);
        }
        printf("]\n");
    }
    if (query_request->contains & TEEP_MESSAGE_CONTAINS_OCSP_DATA) {
        printf("%*socsp-data : ", indent_space + 4, "");
        teep_print_hex(query_request->ocsp_data.ptr, query_request->ocsp_data.len);
        printf("\n");
    }
    printf("%*sdata-item-requested : %u\n", indent_space + 2, "", query_request->data_item_requested);
}

void teep_print_component_id(const teep_buf_t *component_id) {
#ifdef PARSE_SUIT
    QCBORDecodeContext decode_context;
    QCBORDecode_Init(&decode_context, (UsefulBufC){component_id->ptr, component_id->len}, QCBOR_DECODE_MODE_NORMAL);
    QCBORItem item;
    QCBORError error = QCBOR_SUCCESS;
    suit_component_identifier_t identifier;
    int32_t result = SUIT_SUCCESS;
    if (!suit_qcbor_get_next(&decode_context, &item, &error, QCBOR_TYPE_ARRAY)) {
        return;
    }
    result = suit_set_component_identifiers(&decode_context, &item, &error, &identifier);
    if (result != SUIT_SUCCESS) {
        return;
    }
    suit_print_component_identifier(&identifier);
#else
    if (component_id->len <= MAX_PRINT_BYTE_COUNT) {
        teep_print_hex(component_id->ptr, component_id->len);
    }
    else {
        teep_print_hex(component_id->ptr, MAX_PRINT_BYTE_COUNT);
        printf("..");
    }
#endif /* PARSE_SUIT */
}

void print_teep_query_response(const teep_query_response_t *query_response, uint32_t indent_space) {
    printf("%*sQueryResponse :\n", indent_space, "");
    printf("%*stype : %u\n", indent_space + 2, "", query_response->type);
    printf("%*soptions :\n", indent_space + 2, "");
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_TOKEN) {
        printf("%*stoken : %u\n", indent_space + 4, "", query_response->token);
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_SELECTED_CIPHER_SUITE) {
        printf("%*sselected-cipher-suite : %lu\n", indent_space + 4, "", query_response->selected_cipher_suite);
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_SELECTED_VERSION) {
        printf("%*sselected-version : %u\n", indent_space + 4, "", query_response->selected_version);
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_EVIDENCE_FORMAT) {
        printf("%*sevidence-format : ", indent_space + 4, "");
        if (query_response->evidence_format.len <= MAX_PRINT_TEXT_COUNT) {
            teep_print_text(query_response->evidence_format.ptr, query_response->evidence_format.len);
            printf("\n");
        }
        else {
            teep_print_text(query_response->evidence_format.ptr, MAX_PRINT_TEXT_COUNT);
            printf("..\n");
        }
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_EVIDENCE) {
        printf("%*sevidence : ", indent_space + 4, "");
        teep_print_hex(query_response->evidence.ptr, query_response->evidence.len);
        printf("\n");
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_TC_LIST) {
        printf("%*stc-list : [\n", indent_space + 4, "");
        for (size_t i = 0; i < query_response->tc_list.len; i++) {
            printf("%*s{\n", indent_space + 6, "");
            if (query_response->tc_list.items[i].contains & TEEP_MESSAGE_CONTAINS_COMPONENT_ID) {
                printf("          component-id : ");
                teep_print_component_id(&query_response->tc_list.items[i].component_id);
                printf(",\n");
            }
            if (query_response->tc_list.items[i].contains & TEEP_MESSAGE_CONTAINS_TC_MANIFEST_SEQUENCE_NUMBER) {
                if (query_response->tc_list.items[i].tc_manifest_sequence_number != TEEP_SUIT_MANIFEST_SEQUENCE_NUMBER_INVALID) {
                    printf("          tc-manifest-sequence-number : %lu,\n", query_response->tc_list.items[i].tc_manifest_sequence_number);
                }
            }
            printf("%*s},\n", indent_space + 6, "");
        }
        printf("%*s]\n", indent_space + 4, "");
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_EXT_LIST) {
        printf("%*sext-list : [", indent_space + 4, "");
        for (size_t i = 0; i < query_response->ext_list.len; i++) {
            printf("%lu ", query_response->ext_list.items[i]);
        }
        printf("]\n");
    }
}

void print_teep_update(const teep_update_t *teep_update, uint32_t indent_space) {
    printf("%*sUpdate :\n", indent_space, "");
    printf("%*stype : %u\n", indent_space + 2, "", teep_update->type);
    printf("%*soptions :\n", indent_space + 2, "");
    if (teep_update->contains & TEEP_MESSAGE_CONTAINS_TOKEN) {
        printf("%*stoken : %u\n", indent_space + 4, "", teep_update->token);
    }
    if (teep_update->contains & TEEP_MESSAGE_CONTAINS_TC_LIST) {
        printf("%*stc-list : [\n", indent_space + 4, "");
        for (size_t i = 0; i < teep_update->tc_list.len; i++) {
            for (size_t j = 0; j < teep_update->tc_list.len; j++) {
                if (teep_update->tc_list.items[i].len <= MAX_PRINT_BYTE_COUNT) {
                    teep_print_hex(teep_update->tc_list.items[i].ptr, teep_update->tc_list.items[i].len);
                }
                else {
                    teep_print_hex(teep_update->tc_list.items[i].ptr, MAX_PRINT_BYTE_COUNT);
                    printf("..");
                }
                printf(",\n");
            }
            printf("          ]\n");
        }
    }
    if (teep_update->contains & TEEP_MESSAGE_CONTAINS_MANIFEST_LIST) {
        printf("%*smanifest-list : [\n", indent_space + 4, "");
        for (size_t i = 0; i < teep_update->manifest_list.len; i++) {
#ifdef PARSE_SUIT
            QCBORDecodeContext decode_context;
            QCBORDecode_Init(&decode_context, (UsefulBufC){teep_update->manifest_list.items[i].ptr, teep_update->manifest_list.items[i].len}, QCBOR_DECODE_MODE_NORMAL);
            QCBORItem item;
            QCBORError error = QCBOR_SUCCESS;
            suit_envelope_t envelope;
            int32_t result = SUIT_SUCCESS;
            if (!suit_qcbor_get_next(&decode_context, &item, &error, QCBOR_TYPE_ARRAY)) {
                return;
            }
            result = suit_set_envelope(&decode_context, &item, &error, &envelope);
            if (result != SUIT_SUCCESS) {
                return;
            }
            suit_print_envelope(&envelope, 8);
#else
            printf("        ");
            if (teep_update->manifest_list.items[i].len <= MAX_PRINT_BYTE_COUNT) {
                teep_print_hex(teep_update->manifest_list.items[i].ptr, teep_update->manifest_list.items[i].len);
            }
            else {
                teep_print_hex(teep_update->manifest_list.items[i].ptr, MAX_PRINT_BYTE_COUNT);
                printf("..");
            }
            printf(",\n");
#endif /* PARSE_SUIT */
        }
        printf("%*s]\n", indent_space + 4, "");
    }
}

void print_teep_error(const teep_error_t *teep_error, uint32_t indent_space) {
    printf("%*sError :\n", indent_space, "");
    printf("%*stype : %u\n", indent_space + 2, "", teep_error->type);
    printf("%*soptions :\n", indent_space + 2, "");
    if (teep_error->contains & TEEP_MESSAGE_CONTAINS_TOKEN) {
        printf("%*stoken : %u\n", indent_space + 4, "", teep_error->token);
    }
    if (teep_error->contains & TEEP_MESSAGE_CONTAINS_ERR_MSG) {
        printf("%*serr-msg : ", indent_space + 4, "");
        if (teep_error->err_msg.len <= MAX_PRINT_TEXT_COUNT) {
            teep_print_text(teep_error->err_msg.ptr, teep_error->err_msg.len);
            printf("\n");
        }
        else {
            teep_print_text(teep_error->err_msg.ptr, MAX_PRINT_TEXT_COUNT);
            printf("..\n");
        }
    }
    if (teep_error->contains & TEEP_MESSAGE_CONTAINS_SUPPORTED_CIPHER_SUITES) {
        printf("%*ssupported-cipher-suites : ", indent_space + 4, "");
        for (size_t i = 0; i < teep_error->supported_cipher_suites.len; i++) {
            printf("%u ", teep_error->supported_cipher_suites.items[i]);
        }
        printf("\n");
    }
    if (teep_error->contains & TEEP_MESSAGE_CONTAINS_VERSION) {
        printf("%*sversions : ", indent_space + 4, "");
        for (size_t i = 0; i < teep_error->versions.len; i++) {
            printf("%u ", teep_error->versions.items[i]);
        }
        printf("\n");
    }
    if (teep_error->contains & TEEP_MESSAGE_CONTAINS_ERR_CODE) {
        printf("%*serr-code : %u\n", indent_space + 2, "", teep_error->err_code);
    }
    printf("\n");
}

void print_teep_success(const teep_success_t *teep_success, uint32_t indent_space) {
    printf("%*sSuccess :\n", indent_space, "");
    printf("%*stype : %u\n", indent_space + 2, "", teep_success->type);
    printf("%*soptions :\n", indent_space + 2, "");
    if (teep_success->contains & TEEP_MESSAGE_CONTAINS_TOKEN) {
        printf("%*stoken : %u\n", indent_space + 4, "", teep_success->token);
    }
    if (teep_success->contains & TEEP_MESSAGE_CONTAINS_MSG) {
        printf("%*smsg : ", indent_space + 2, "");
        if (teep_success->msg.len <= MAX_PRINT_TEXT_COUNT) {
            teep_print_text(teep_success->msg.ptr, teep_success->msg.len);
            printf("\n");
        }
        else {
            teep_print_text(teep_success->msg.ptr, MAX_PRINT_TEXT_COUNT);
            printf("..\n");
        }
    }
}

void print_teep_message(const uint8_t *message, const size_t message_len, uint32_t indent_space) {
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
            print_teep_query_request(&d.query_request, indent_space);
            break;
        case TEEP_TYPE_QUERY_RESPONSE:
            set_teep_query_response(&decode_context, &d.query_response);
            print_teep_query_response(&d.query_response, indent_space);
            break;
        case TEEP_TYPE_UPDATE:
            set_teep_update(&decode_context, &d.teep_update);
            print_teep_update(&d.teep_update, indent_space);
            break;
        case TEEP_TYPE_TEEP_SUCCESS:
            set_teep_success(&decode_context, &d.teep_success);
            print_teep_success(&d.teep_success, indent_space);
            break;
        case TEEP_TYPE_TEEP_ERROR:
            set_teep_error(&decode_context, &d.teep_error);
            print_teep_error(&d.teep_error, indent_space);
            break;
        default:
            teep_print_debug_string("print_teep_message : Undefined value.\n");
            break;
    }
    QCBORDecode_Finish(&decode_context);
}
