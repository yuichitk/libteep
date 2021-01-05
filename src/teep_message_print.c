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

int32_t teep_print_hex_within_max(const uint8_t *array, const size_t size, const size_t size_max) {
    if (array == NULL) {
        return TEEP_UNEXPECTED_ERROR;
    }
    size_t print_len = (size < size_max) ? size : size_max;
    for (size_t i = 0; i < print_len; i++) {
        printf("0x%02x ", (unsigned char)array[i]);
    }
    if (print_len < size) {
        printf("..");
    }
    return TEEP_SUCCESS;
}

int32_t teep_print_hex(const uint8_t *array, const size_t size) {
    return teep_print_hex_within_max(array, size, TEEP_MAX_PRINT_BYTE_COUNT);
}

int32_t teep_print_text(const uint8_t *text, const size_t size) {
    if (text == NULL) {
        return TEEP_UNEXPECTED_ERROR;
    }
    size_t print_len = (size < TEEP_MAX_PRINT_TEXT_COUNT) ? size : TEEP_MAX_PRINT_TEXT_COUNT;
    printf("\"");
    for (size_t j = 0; j < print_len; j++) {
        putchar(text[j]);
    }
    printf("\"");
    if (print_len < size) {
        printf("..");
    }
    return TEEP_SUCCESS;
}

int32_t teep_print_string(const teep_buf_t *string) {
    return teep_print_text(string->ptr, string->len);
}

void teep_debug_print(QCBORDecodeContext *message,
                      QCBORItem *item,
                      QCBORError *error,
                      const char *func_name,
                      uint8_t expecting) {
    size_t cursor = UsefulInputBuf_Tell(&message->InBuf);
    size_t len = UsefulInputBuf_GetBufferLength(&message->InBuf) - cursor;
    uint8_t *at = (uint8_t *)message->InBuf.UB.ptr + cursor;

    len = (len > 12) ? 12 : len;

    printf("DEBUG: %s\n", func_name);
    printf("msg[%ld:%ld] = ", cursor, cursor + len);
    teep_print_hex_within_max(at, len, len);
    printf("\n");

    if (error != NULL && *error != QCBOR_SUCCESS) {
        printf("    Error! nCBORError = %d\n", *error);
    }
    if (expecting != QCBOR_TYPE_ANY && expecting != item->uDataType) {
        printf("    item->uDataType %d != %d\n", item->uDataType, expecting);
    }
}


int32_t print_teep_query_request(const teep_query_request_t *query_request, uint32_t indent_space) {
    if (query_request == NULL) {
        return TEEP_UNEXPECTED_ERROR;
    }
    int32_t result = TEEP_SUCCESS;
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
        result = teep_print_hex(query_request->challenge.ptr, query_request->challenge.len);
        if (result != TEEP_SUCCESS) {
            return result;
        }
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
        result = teep_print_hex(query_request->ocsp_data.ptr, query_request->ocsp_data.len);
        if (result != TEEP_SUCCESS) {
            return result;
        }
        printf("\n");
    }
    printf("%*sdata-item-requested : %u\n", indent_space + 2, "", query_request->data_item_requested);
    return TEEP_SUCCESS;
}

int32_t teep_print_component_id(const teep_buf_t *component_id) {
    if (component_id == NULL) {
        return TEEP_UNEXPECTED_ERROR;
    }
    int32_t result = TEEP_SUCCESS;
#ifdef PARSE_SUIT
    suit_buf_t buf = {.ptr = component_id->ptr, .len = component_id->len};
    suit_component_identifier_t identifier;
    int32_t suit_result = suit_set_component_identifiers(SUIT_DECODE_MODE_SKIP_ANY_ERROR, &buf, &identifier);
    if (suit_result != SUIT_SUCCESS) {
        return TEEP_UNEXPECTED_ERROR;
    }
    suit_result = suit_print_component_identifier(&identifier);
    if (suit_result != SUIT_SUCCESS) {
        return TEEP_UNEXPECTED_ERROR;
    }
#else
    result = teep_print_hex(component_id->ptr, component_id->len);
#endif /* PARSE_SUIT */
    return result;
}

int32_t print_teep_query_response(const teep_query_response_t *query_response, uint32_t indent_space) {
    if (query_response == NULL) {
        return TEEP_UNEXPECTED_ERROR;
    }
    int32_t result = TEEP_SUCCESS;
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
        result = teep_print_string(&query_response->evidence_format);
        if (result != TEEP_SUCCESS) {
            return result;
        }
        printf("\n");
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_EVIDENCE) {
        printf("%*sevidence : ", indent_space + 4, "");
        result = teep_print_hex(query_response->evidence.ptr, query_response->evidence.len);
        if (result != TEEP_SUCCESS) {
            return result;
        }
        printf("\n");
    }
    if (query_response->contains & TEEP_MESSAGE_CONTAINS_TC_LIST) {
        printf("%*stc-list : [\n", indent_space + 4, "");
        for (size_t i = 0; i < query_response->tc_list.len; i++) {
            printf("%*s{\n", indent_space + 6, "");
            if (query_response->tc_list.items[i].contains & TEEP_MESSAGE_CONTAINS_COMPONENT_ID) {
                printf("%*scomponent-id : ", indent_space + 8, "");
                result = teep_print_component_id(&query_response->tc_list.items[i].component_id);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                printf(",\n");
            }
            if (query_response->tc_list.items[i].contains & TEEP_MESSAGE_CONTAINS_TC_MANIFEST_SEQUENCE_NUMBER) {
                if (query_response->tc_list.items[i].tc_manifest_sequence_number != TEEP_SUIT_MANIFEST_SEQUENCE_NUMBER_INVALID) {
                    printf("%*stc-manifest-sequence-number : %lu,\n", indent_space + 8, "", query_response->tc_list.items[i].tc_manifest_sequence_number);
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
    return TEEP_SUCCESS;
}

int32_t print_teep_update(const teep_update_t *teep_update, uint32_t indent_space, const char *ta_public_key) {
    if (teep_update == NULL) {
        return TEEP_UNEXPECTED_ERROR;
    }
    int32_t result = TEEP_SUCCESS;
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
                result = teep_print_hex(teep_update->tc_list.items[i].ptr, teep_update->tc_list.items[i].len);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                printf(",\n");
            }
        }
        printf("%*s]\n", indent_space + 4, "");
    }
    if (teep_update->contains & TEEP_MESSAGE_CONTAINS_MANIFEST_LIST) {
        printf("%*smanifest-list : [\n", indent_space + 4, "");
        for (size_t i = 0; i < teep_update->manifest_list.len; i++) {
#ifdef PARSE_SUIT
            suit_buf_t buf = {.ptr = teep_update->manifest_list.items[i].ptr, .len = teep_update->manifest_list.items[i].len};
            suit_envelope_t envelope;
            int32_t suit_result = suit_set_envelope(SUIT_DECODE_MODE_SKIP_ANY_ERROR, &buf, &envelope, ta_public_key);
            if (suit_result != SUIT_SUCCESS) {
                return TEEP_UNEXPECTED_ERROR;
            }
            suit_result = suit_print_envelope(SUIT_DECODE_MODE_SKIP_ANY_ERROR, &envelope, indent_space + 6);
            if (result != SUIT_SUCCESS) {
                return TEEP_UNEXPECTED_ERROR;
            }
#else
            printf("%*s", indent_space + 6, "");
            result = teep_print_hex(teep_update->manifest_list.items[i].ptr, teep_update->manifest_list.items[i].len);
            if (result != TEEP_SUCCESS) {
                return result;
            }
            printf(",\n");
#endif /* PARSE_SUIT */
        }
        printf("%*s]\n", indent_space + 4, "");
    }
    return TEEP_SUCCESS;
}

int32_t print_teep_error(const teep_error_t *teep_error, uint32_t indent_space) {
    if (teep_error == NULL) {
        return TEEP_UNEXPECTED_ERROR;
    }
    int32_t result = TEEP_SUCCESS;
    printf("%*sError :\n", indent_space, "");
    printf("%*stype : %u\n", indent_space + 2, "", teep_error->type);
    printf("%*soptions :\n", indent_space + 2, "");
    if (teep_error->contains & TEEP_MESSAGE_CONTAINS_TOKEN) {
        printf("%*stoken : %u\n", indent_space + 4, "", teep_error->token);
    }
    if (teep_error->contains & TEEP_MESSAGE_CONTAINS_ERR_MSG) {
        printf("%*serr-msg : ", indent_space + 4, "");
        result = teep_print_text(teep_error->err_msg.ptr, teep_error->err_msg.len);
        if (result != TEEP_SUCCESS) {
            return result;
        }
        printf("\n");
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
    return TEEP_SUCCESS;
}

int32_t print_teep_success(const teep_success_t *teep_success, uint32_t indent_space) {
    if (teep_success == NULL) {
        return TEEP_UNEXPECTED_ERROR;
    }
    int32_t result;
    printf("%*sSuccess :\n", indent_space, "");
    printf("%*stype : %u\n", indent_space + 2, "", teep_success->type);
    printf("%*soptions :\n", indent_space + 2, "");
    if (teep_success->contains & TEEP_MESSAGE_CONTAINS_TOKEN) {
        printf("%*stoken : %u\n", indent_space + 4, "", teep_success->token);
    }
    if (teep_success->contains & TEEP_MESSAGE_CONTAINS_MSG) {
        printf("%*smsg : ", indent_space + 2, "");
        result = teep_print_text(teep_success->msg.ptr, teep_success->msg.len);
        if (result != TEEP_SUCCESS) {
            return result;
        }
        printf("\n");
    }
    return TEEP_SUCCESS;
}

int32_t print_teep_message(const teep_message_t *msg, uint32_t indent_space, const char *ta_public_key) {
    if (msg == NULL) {
        return TEEP_UNEXPECTED_ERROR;
    }
    int32_t result = TEEP_SUCCESS;
    switch (msg->teep_message.type) {
        case TEEP_TYPE_QUERY_REQUEST:
            result = print_teep_query_request(&msg->query_request, indent_space);
            break;
        case TEEP_TYPE_QUERY_RESPONSE:
            result = print_teep_query_response(&msg->query_response, indent_space);
            break;
        case TEEP_TYPE_UPDATE:
            result = print_teep_update(&msg->teep_update, indent_space, ta_public_key);
            break;
        case TEEP_TYPE_TEEP_SUCCESS:
            result = print_teep_success(&msg->teep_success, indent_space);
            break;
        case TEEP_TYPE_TEEP_ERROR:
            result = print_teep_error(&msg->teep_error, indent_space);
            break;
        default:
            result = TEEP_UNEXPECTED_ERROR;
            teep_print_debug_string("print_teep_message : Undefined value.\n");
            break;
    }
    return result;
}
