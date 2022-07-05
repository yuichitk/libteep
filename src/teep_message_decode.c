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

teep_err_t teep_qcbor_get_next(QCBORDecodeContext *message,
                         QCBORItem *item,
                         uint8_t data_type) {
    QCBORError error = QCBORDecode_GetNext(message, item);
    switch (error) {
        case QCBOR_SUCCESS:
            break;
        case QCBOR_ERR_NO_MORE_ITEMS:
        default:
            return TEEP_ERR_UNEXPECTED_ERROR;
    }
    if (data_type != QCBOR_TYPE_ANY && item->uDataType != data_type) {
        return TEEP_ERR_INVALID_TYPE_OF_ARGUMENT;
    }
    return TEEP_SUCCESS;
}

bool teep_qcbor_get_next_uint(QCBORDecodeContext *message,
                              QCBORItem *item) {
    if (teep_qcbor_get_next(message, item, QCBOR_TYPE_ANY) == TEEP_SUCCESS) {
        return false;
    }
    if (item->uDataType == QCBOR_TYPE_INT64) {
        if (item->val.int64 < 0) {
            return false;
        }
    }
    else if (item->uDataType != QCBOR_TYPE_UINT64) {
        return false;
    }
    return true;
}

bool teep_qcbor_skip_any(QCBORDecodeContext *message, QCBORItem *item);

bool teep_qcbor_skip_array_and_map(QCBORDecodeContext *message, QCBORItem *item) {
    if (item->uDataType != QCBOR_TYPE_ARRAY && item->uDataType != QCBOR_TYPE_MAP) {
        return false;
    }
    size_t array_size = item->val.uCount;
    for (size_t i = 0; i < array_size; i++) {
        if (teep_qcbor_get_next(message, item, QCBOR_TYPE_ANY) != TEEP_SUCCESS) {
            return false;
        }
        if (!teep_qcbor_skip_any(message, item)) {
            return false;
        }
    }
    return true;
}

bool teep_qcbor_skip_any(QCBORDecodeContext *message, QCBORItem *item) {
    switch (item->uDataType) {
        case QCBOR_TYPE_ARRAY:
        case QCBOR_TYPE_MAP:
            if (!teep_qcbor_skip_array_and_map(message, item)) {
                return false;
            }
            break;
        case QCBOR_TYPE_INT64:
        case QCBOR_TYPE_UINT64:
        case QCBOR_TYPE_BYTE_STRING:
        case QCBOR_TYPE_TEXT_STRING:
            break;
        default:
            return false;
    }
    return true;
}

bool teep_qcbor_value_is_uint64(QCBORItem *item) {
    if (item->uDataType == QCBOR_TYPE_INT64) {
        if (item->val.int64 < 0) {
            return false;
        }
        /* there is no need to cast int64_t [0, INT32_MAX] value into uint64_t in the union */
    }
    else if (item->uDataType != QCBOR_TYPE_UINT64) {
        return false;
    }
    return true;
}

bool teep_qcbor_value_is_uint32(QCBORItem *item) {
    switch (item->uDataType) {
        case QCBOR_TYPE_INT64:
            if (item->val.int64 < 0 || item->val.int64 > UINT32_MAX) {
                return false;
            }
            break;
        case QCBOR_TYPE_UINT64:
            if (item->val.uint64 > UINT32_MAX) {
                return false;
            }
            break;
        default:
            return false;
    }
    return true;
}

/* As QCBOR parse [0, INT32_MAX] values as QCBOR_TYPE_INT64,
 * we need to distinguish the number is in [0, INT64_MAX].
 */
teep_err_t teep_qcbor_get_next_uint64(QCBORDecodeContext *message, QCBORItem *item) {
    teep_err_t result = teep_qcbor_get_next(message, item, QCBOR_TYPE_ANY);
    if (result != TEEP_SUCCESS) {
        return result;
    }
    if (teep_qcbor_value_is_uint64(item)) {
        return TEEP_SUCCESS;
    }
    return TEEP_ERR_INVALID_TYPE_OF_ARGUMENT;
}

uint64_t teep_get_message_type(QCBORDecodeContext *message) {
    QCBORItem item;
    if (teep_qcbor_get_next(message, &item, QCBOR_TYPE_ARRAY) != TEEP_SUCCESS) {
        return TEEP_TYPE_INVALID;
    }
    if (teep_qcbor_get_next_uint64(message, &item)) {
        return TEEP_TYPE_INVALID;
    }
    return item.val.uint64;
}

/*
 * counts the CBOR binary offset between the CBOR type and length declaration
 * and current cursor = UsefulInputBuf_Tell(&context.InBuf).
 * note that the cursor point the current item's next buffer.
 * with INT64, UINT64, TEXT_STRING, and BYTE_STRING,
 * the current cursor is next to the tail of the value.
 * 0F               # unsigned(15)
 * O ^
 * 38 18            # negative(24) = -25
 * O    ^
 * however, with ARRAY, MAP, MAP_AS_ARRAY,
 * the current cursor is next to the tail of the type and length declaration.
 * 82 01 02         # Array(1) [ unsigned(1), unsigned(2) ]
 * O  ^
 * A1 01 82 02 02   # Map(1) { unsigned(1) : Array(2) [ unsigned(2), unsigned(2) ] }
 * O  ^
 */
size_t teep_qcbor_calc_rollback(QCBORItem *item) {
    uint8_t type = item->uDataType;
    if (item->uDataType == QCBOR_TYPE_INT64 && teep_qcbor_value_is_uint64(item)) {
        type = QCBOR_TYPE_UINT64;
    }

    switch (type) {
        case QCBOR_TYPE_UINT64:
            if (item->val.uint64 <= 23) {
                return 1;
            }
            else if (item->val.uint64 <= UINT8_MAX) {
                return 2;
            }
            else if (item->val.uint64 <= UINT16_MAX) {
                return 3;
            }
            else if (item->val.uint64 <= UINT32_MAX) {
                return 5;
            }
            return 9;
        case QCBOR_TYPE_INT64:
            if (item->val.int64 + 1 + 23 >= 0) {
                return 1;
            }
            else if (item->val.int64 + 1 + UINT8_MAX >= 0) {
                return 2;
            }
            else if (item->val.int64 + 1 + UINT16_MAX >= 0) {
                return 3;
            }
            else if (item->val.int64 + 1 + UINT32_MAX >= 0) {
                return 5;
            }
            return 9;
        case QCBOR_TYPE_BYTE_STRING:
        case QCBOR_TYPE_TEXT_STRING:
            if (item->val.string.len < 24) {
                return 1 + item->val.string.len;
            }
            else if (item->val.string.len <= UINT8_MAX) {
                return 2 + item->val.string.len;
            }
            else if (item->val.string.len <= UINT16_MAX) {
                return 3 + item->val.string.len;
            }
            else if (item->val.string.len <= UINT32_MAX) {
                return 5 + item->val.string.len;
            }
            return 9 + item->val.string.len;
        case QCBOR_TYPE_ARRAY:
        case QCBOR_TYPE_MAP:
            if (item->val.uCount < 24) {
                return 1;
            }
            else if (item->val.uCount < UINT8_MAX) {
                return 2;
            }
            else if (item->val.uCount < UINT16_MAX) {
                return 3;
            }
            else if (item->val.uCount < UINT32_MAX) {
                return 5;
            }
            return 9;
    }
    return 0;
}

/*
 * set ptr and skip out of teep message cbor part such as suit-report
 */
teep_err_t teep_set_out_of_teep_buf(QCBORDecodeContext *message,
                                 QCBORItem *item,
                                 teep_buf_t *teep_buf) {
    /* roll back the cursor locally,
     * because QCBORDecode_GetNext() set the cursor after depends on the context.
     */
    uint8_t *buf;
    size_t rollback = teep_qcbor_calc_rollback(item);
    if (rollback == 0) {
        return TEEP_ERR_UNEXPECTED_ERROR;
    }
    size_t cursor = UsefulInputBuf_Tell(&message->InBuf);
    buf = (uint8_t *)message->InBuf.UB.ptr + (cursor - rollback);
    if (!teep_qcbor_skip_any(message, item)) {
        return TEEP_ERR_UNEXPECTED_ERROR;
    }
    teep_buf->ptr = buf;
    teep_buf->len = UsefulInputBuf_Tell(&message->InBuf) - (cursor - rollback);

    return TEEP_SUCCESS;
}

teep_err_t teep_set_any_array(QCBORDecodeContext *message,
                           QCBORItem *item,
                           uint8_t targetDataType,
                           void *teep_any_array) {
    union {
        void *v;
        teep_uint64_array_t *u64;
        teep_uint32_array_t *u32;
        teep_buf_array_t *b;
    } ptr;
    ptr.v = teep_any_array;

    if (item->uDataType != QCBOR_TYPE_ARRAY) {
        return TEEP_ERR_INVALID_TYPE_OF_ARGUMENT;
    }
    if (item->val.uCount > TEEP_MAX_ARRAY_LENGTH) {
        return TEEP_ERR_NO_MEMORY;
    }
    size_t array_size = item->val.uCount;
    for (size_t j = 0; j < array_size; j++) {
        teep_err_t result = teep_qcbor_get_next(message, item, QCBOR_TYPE_ANY);
        if (result != TEEP_SUCCESS) {
            return result;
        }
        switch(targetDataType) {
            case QCBOR_TYPE_UINT64:
                if (!teep_qcbor_value_is_uint64(item)) {
                    return TEEP_ERR_INVALID_TYPE_OF_ARGUMENT;
                }
                ptr.u64->items[j] = item->val.uint64;
                ptr.u64->len = j + 1;
                break;
            case QCBOR_TYPE_UINT32:
                /* extended type */
                if (!teep_qcbor_value_is_uint32(item)) {
                    return TEEP_ERR_INVALID_TYPE_OF_ARGUMENT;
                }
                ptr.u32->items[j] = (uint32_t)item->val.uint64;
                ptr.u32->len = j + 1;
                break;
            case QCBOR_TYPE_BYTE_STRING:
                if (item->uDataType != QCBOR_TYPE_BYTE_STRING) {
                    return TEEP_ERR_INVALID_TYPE_OF_ARGUMENT;
                }
                ptr.b->items[j].ptr = item->val.string.ptr;
                ptr.b->items[j].len = item->val.string.len;
                ptr.b->len = j + 1;
                break;
            case QCBOR_TYPE_ANY:
                result = teep_set_out_of_teep_buf(message, item, &ptr.b->items[j]);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                ptr.b->len = j + 1;
                break;
            default:
                break;
        }
    }
    return TEEP_SUCCESS;
}

teep_err_t teep_set_byte_string(uint8_t data_type,
                             QCBORItem *item,
                             teep_buf_t *buf) {
    if (item->uDataType != data_type) {
        return TEEP_ERR_INVALID_TYPE_OF_ARGUMENT;
    }
    buf->ptr = item->val.string.ptr;
    buf->len = item->val.string.len;
    return TEEP_SUCCESS;
}

teep_err_t teep_set_ciphersuite(QCBORDecodeContext *message,
                             QCBORItem *item,
                             teep_ciphersuite_t *ciphersuite) {
    if (item->uDataType != QCBOR_TYPE_ARRAY) {
        return TEEP_ERR_INVALID_TYPE_OF_ARGUMENT;
    }
    if (item->val.uCount != TEEP_CIPHERSUITE_LENGTH) {
        return TEEP_ERR_NO_MEMORY;
    }

    for (size_t i = 0; i < TEEP_CIPHERSUITE_LENGTH; i++) {
        teep_err_t result = teep_qcbor_get_next(message, item, QCBOR_TYPE_ANY);
        if (result != TEEP_SUCCESS) {
            return result;
        }
        switch (item->uDataType) {
        case QCBOR_TYPE_UINT64:
            switch (i) {
            case 0: /* sign */
                ciphersuite->sign = item->val.uint64;
                break;
            case 1: /* encrypt */
                ciphersuite->encrypt = item->val.uint64;
                break;
            case 2: /* mac */
                ciphersuite->mac = item->val.uint64;
                break;
            default:
                return TEEP_ERR_INVALID_LENGTH;
            }
            break;
        case QCBOR_TYPE_INT64:
            switch (i) {
            case 0: /* sign */
                ciphersuite->sign = item->val.int64;
                break;
            case 1: /* encrypt */
                ciphersuite->encrypt = item->val.int64;
                break;
            case 2: /* mac */
                ciphersuite->mac = item->val.int64;
                break;
            default:
                return TEEP_ERR_INVALID_LENGTH;
            }
            break;
        case QCBOR_TYPE_NULL:
            switch (i) {
            case 0: /* sign */
                ciphersuite->sign = TEEP_COSE_SIGN_NONE;
                break;
            case 1: /* encrypt */
                ciphersuite->encrypt = TEEP_COSE_ENCRYPT_NONE;
                break;
            case 2: /* mac */
                ciphersuite->mac = TEEP_COSE_MAC_NONE;
                break;
            default:
                return TEEP_ERR_INVALID_LENGTH;
            }
            break;
        default:
            return TEEP_ERR_INVALID_TYPE_OF_ARGUMENT;
        }
    }

    return TEEP_SUCCESS;
}

teep_err_t teep_set_ciphersuite_array(QCBORDecodeContext *message,
                                   QCBORItem *item,
                                   teep_ciphersuite_array_t *ciphersuites) {
    if (item->uDataType != QCBOR_TYPE_ARRAY) {
        return TEEP_ERR_INVALID_TYPE_OF_ARGUMENT;
    }
    if (item->val.uCount > TEEP_MAX_ARRAY_LENGTH) {
        return TEEP_ERR_NO_MEMORY;
    }
    size_t array_size = item->val.uCount;
    for (size_t i = 0 ; i < array_size; i++) {
        teep_err_t result = teep_qcbor_get_next(message, item, QCBOR_TYPE_ARRAY);
        if (result != TEEP_SUCCESS) {
            return result;
        }

        result = teep_set_ciphersuite(message, item, &ciphersuites->items[i]);
        if (result != TEEP_SUCCESS) {
            return result;
        }
    }
    ciphersuites->len = array_size;

    return TEEP_SUCCESS;
}

teep_err_t teep_set_tc_info_array(QCBORDecodeContext *message,
                               QCBORItem *item,
                               teep_tc_info_array_t *tc_info_arr) {
    teep_err_t result = TEEP_SUCCESS;

    INITIALIZE_TEEP_ARRAY_PTR(tc_info_arr);

    if (item->uDataType != QCBOR_TYPE_ARRAY) {
        return TEEP_ERR_INVALID_TYPE_OF_ARGUMENT;
    }
    if (item->val.uCount > TEEP_MAX_ARRAY_LENGTH) {
        return TEEP_ERR_NO_MEMORY;
    }
    tc_info_arr->len = item->val.uCount;
    for (size_t i = 0; i < tc_info_arr->len; i++) {
        result = teep_qcbor_get_next(message, item, QCBOR_TYPE_MAP);
        if (result != TEEP_SUCCESS) {
            tc_info_arr->len = i;
            return result;
        }
        size_t map_count = item->val.uCount;
        for (size_t j = 0; j < map_count; j++) {
            result = teep_qcbor_get_next(message, item, QCBOR_TYPE_ANY);
            if (result != TEEP_SUCCESS) {
                break;
            }
            tc_info_arr->items[i].contains = 0UL;
            tc_info_arr->items[i].tc_manifest_sequence_number = TEEP_SUIT_MANIFEST_SEQUENCE_NUMBER_INVALID;
            switch (item->label.uint64) {
                case TEEP_OPTIONS_KEY_COMPONENT_ID:
                    /* SUIT_Component_Identifier = [ +bstr ] (currently) */
                    if (item->uDataType != QCBOR_TYPE_ARRAY) {
                        result = TEEP_ERR_INVALID_TYPE_OF_ARGUMENT;
                        break;
                    }
                    result = teep_set_out_of_teep_buf(message, item, &tc_info_arr->items[i].component_id);
                    if (result != TEEP_SUCCESS) {
                        break;
                    }
                    tc_info_arr->items[i].contains |= TEEP_MESSAGE_CONTAINS_COMPONENT_ID;
                    break;
                case TEEP_OPTIONS_KEY_TC_MANIFEST_SEQUENCE_NUMBER:
                    if (!teep_qcbor_value_is_uint64(item)) {
                        result = TEEP_ERR_INVALID_TYPE_OF_ARGUMENT;
                        break;
                    }
                    tc_info_arr->items[i].tc_manifest_sequence_number = item->val.uint64;
                    tc_info_arr->items[i].contains |= TEEP_MESSAGE_CONTAINS_TC_MANIFEST_SEQUENCE_NUMBER;
                    break;
                default:
                    break;
            }
            if (result != TEEP_SUCCESS) {
                tc_info_arr->len = i;
                return result;
            }
        }
    }
    return TEEP_SUCCESS;
}

teep_err_t teep_set_requested_tc_info_array(QCBORDecodeContext *message,
                                         QCBORItem *item,
                                         teep_requested_tc_info_array_t *tc_info_arr) {
    teep_err_t result = TEEP_SUCCESS;

    INITIALIZE_TEEP_ARRAY_PTR(tc_info_arr);

    if (item->uDataType != QCBOR_TYPE_ARRAY) {
        return TEEP_ERR_INVALID_TYPE_OF_ARGUMENT;
    }
    if (item->val.uCount > TEEP_MAX_ARRAY_LENGTH) {
        return TEEP_ERR_NO_MEMORY;
    }
    tc_info_arr->len = item->val.uCount;
    for (size_t i = 0; i < tc_info_arr->len; i++) {
        result = teep_qcbor_get_next(message, item, QCBOR_TYPE_MAP);
        if (result != TEEP_SUCCESS) {
            tc_info_arr->len = i;
            return result;
        }
        size_t map_count = item->val.uCount;
        for (size_t j = 0; j < map_count; j++) {
            result = teep_qcbor_get_next(message, item, QCBOR_TYPE_ANY);
            if (result != TEEP_SUCCESS) {
                break;
            }
            tc_info_arr->items[i].contains = 0UL;
            tc_info_arr->items[i].tc_manifest_sequence_number = TEEP_SUIT_MANIFEST_SEQUENCE_NUMBER_INVALID;
            switch (item->label.uint64) {
                case TEEP_OPTIONS_KEY_COMPONENT_ID:
                    /* SUIT_Component_Identifier = [ +bstr ] (currently) */
                    result = teep_set_out_of_teep_buf(message, item, &tc_info_arr->items[i].component_id);
                    if (result != TEEP_SUCCESS) {
                        break;
                    }
                    tc_info_arr->items[i].contains |= TEEP_MESSAGE_CONTAINS_COMPONENT_ID;
                    break;
                case TEEP_OPTIONS_KEY_TC_MANIFEST_SEQUENCE_NUMBER:
                    if (!teep_qcbor_value_is_uint64(item)) {
                        result = TEEP_ERR_INVALID_TYPE_OF_ARGUMENT;
                        break;
                    }
                    tc_info_arr->items[i].tc_manifest_sequence_number = item->val.uint64;
                    tc_info_arr->items[i].contains |= TEEP_MESSAGE_CONTAINS_TC_MANIFEST_SEQUENCE_NUMBER;
                    break;
                case TEEP_OPTIONS_KEY_HAVE_BINARY:
                    if (item->uDataType != QCBOR_TYPE_TRUE && item->uDataType != QCBOR_TYPE_FALSE) {
                        result = TEEP_ERR_INVALID_TYPE_OF_ARGUMENT;
                        break;
                    }
                    tc_info_arr->items[i].have_binary = (bool)item->val.uint64;
                    tc_info_arr->items[i].contains |= TEEP_MESSAGE_CONTAINS_HAVE_BINARY;
                    break;
                default:
                    result = TEEP_ERR_UNEXPECTED_ERROR;
            }
            if (result != TEEP_SUCCESS) {
                tc_info_arr->len = i;
                return result;
            }
        }
    }
    return TEEP_SUCCESS;
}

teep_err_t teep_set_query_request(QCBORDecodeContext *message,
                               QCBORItem *item,
                               teep_query_request_t *query_request) {
    teep_err_t result = TEEP_SUCCESS;

    query_request->token.ptr = NULL;
    query_request->token.len = 0;
    INITIALIZE_TEEP_ARRAY(query_request->supported_cipher_suites);
    INITIALIZE_TEEP_ARRAY(query_request->supported_freshness_mechanisms);
    INITIALIZE_TEEP_BUF(query_request->challenge);
    INITIALIZE_TEEP_ARRAY(query_request->versions);
    query_request->data_item_requested = TEEP_DATA_ITEM_INVALID;

    result = teep_qcbor_get_next(message, item, QCBOR_TYPE_MAP);
    if (result != TEEP_SUCCESS) {
        return result;
    }
    size_t map_count = item->val.uCount;
    for (size_t i = 0; i < map_count; i++) {
        result = teep_qcbor_get_next(message, item, QCBOR_TYPE_ANY);
        if (result != TEEP_SUCCESS) {
            return result;
        }
        switch (item->label.uint64) {
            case TEEP_OPTIONS_KEY_TOKEN:
                result = teep_set_byte_string(QCBOR_TYPE_BYTE_STRING, item, &query_request->token);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_request->contains |= TEEP_MESSAGE_CONTAINS_TOKEN;
                break;
            case TEEP_OPTIONS_KEY_SUPPORTED_CIPHER_SUITES:
                result = teep_set_ciphersuite_array(message, item, &query_request->supported_cipher_suites);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_request->contains |= TEEP_MESSAGE_CONTAINS_SUPPORTED_CIPHER_SUITES;
                break;
            case TEEP_OPTIONS_KEY_CHALLENGE:
                result = teep_set_byte_string(QCBOR_TYPE_BYTE_STRING, item, &query_request->challenge);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_request->contains |= TEEP_MESSAGE_CONTAINS_CHALLENGE;
                break;
            case TEEP_OPTIONS_KEY_VERSION:
                result = teep_set_any_array(message, item, QCBOR_TYPE_UINT32, &query_request->versions);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_request->contains |= TEEP_MESSAGE_CONTAINS_VERSION;
                break;
            case TEEP_OPTIONS_KEY_SUPPORTED_FRESHNESS_MECHANISMS:
                result = teep_set_any_array(message, item, QCBOR_TYPE_UINT32, &query_request->supported_freshness_mechanisms);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_request->contains |= TEEP_MESSAGE_CONTAINS_SUPPORTED_FRESHNESS_MECHANISMS;
                break;
            default:
                return TEEP_ERR_UNEXPECTED_ERROR;
        }
    }
    result = teep_qcbor_get_next_uint64(message, item);
    if (result != TEEP_SUCCESS) {
        return result;
    }
    query_request->data_item_requested = item->val.uint64;
    return TEEP_SUCCESS;
}

teep_err_t teep_set_query_response(QCBORDecodeContext *message,
                                QCBORItem *item,
                                teep_query_response_t *query_response) {
    teep_err_t result = TEEP_SUCCESS;

    query_response->token.ptr = NULL;
    query_response->token.len = 0;
    query_response->selected_cipher_suite = TEEP_CIPHERSUITE_INVALID;
    query_response->selected_version = 0U;
    INITIALIZE_TEEP_BUF(query_response->evidence_format);
    INITIALIZE_TEEP_BUF(query_response->evidence);
    INITIALIZE_TEEP_ARRAY(query_response->tc_list);
    INITIALIZE_TEEP_ARRAY(query_response->requested_tc_list);
    INITIALIZE_TEEP_ARRAY(query_response->unneeded_tc_list);
    INITIALIZE_TEEP_ARRAY(query_response->ext_list);

    result = teep_qcbor_get_next(message, item, QCBOR_TYPE_MAP);
    if (result != TEEP_SUCCESS) {
        return result;
    }
    size_t map_count = item->val.uCount;
    for (size_t i = 0; i < map_count; i++) {
        result = teep_qcbor_get_next(message, item, QCBOR_TYPE_ANY);
        if (result != TEEP_SUCCESS) {
            return result;
        }
        switch (item->label.uint64) {
            case TEEP_OPTIONS_KEY_TOKEN:
                result = teep_set_byte_string(QCBOR_TYPE_BYTE_STRING, item, &query_response->token);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_response->contains |= TEEP_MESSAGE_CONTAINS_TOKEN;
                break;
            case TEEP_OPTIONS_KEY_SELECTED_CIPHER_SUITE:
                result = teep_set_ciphersuite(message, item, &query_response->selected_cipher_suite);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_response->contains |= TEEP_MESSAGE_CONTAINS_SELECTED_CIPHER_SUITE;
                break;
            case TEEP_OPTIONS_KEY_SELECTED_VERSION:
                if (!teep_qcbor_value_is_uint32(item)) {
                    return TEEP_ERR_INVALID_TYPE_OF_ARGUMENT;
                }
                query_response->selected_version = (uint32_t)item->val.uint64;
                query_response->contains |= TEEP_MESSAGE_CONTAINS_SELECTED_VERSION;
                break;
            case TEEP_OPTIONS_KEY_EVIDENCE_FORMAT:
                result = teep_set_byte_string(QCBOR_TYPE_TEXT_STRING, item, &query_response->evidence_format);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_response->contains |= TEEP_MESSAGE_CONTAINS_EVIDENCE_FORMAT;
                break;
            case TEEP_OPTIONS_KEY_EVIDENCE:
                result = teep_set_byte_string(QCBOR_TYPE_BYTE_STRING, item, &query_response->evidence);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_response->contains |= TEEP_MESSAGE_CONTAINS_EVIDENCE;
                break;
            case TEEP_OPTIONS_KEY_TC_LIST:
                result = teep_set_tc_info_array(message, item, &query_response->tc_list);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_response->contains |= TEEP_MESSAGE_CONTAINS_TC_LIST;
                break;
            case TEEP_OPTIONS_KEY_REQUESTED_TC_LIST:
                result = teep_set_requested_tc_info_array(message, item, &query_response->requested_tc_list);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_response->contains |= TEEP_MESSAGE_CONTAINS_REQUESTED_TC_LIST;
                break;
            case TEEP_OPTIONS_KEY_UNNEEDED_TC_LIST:
                /* [ + SUIT_Component_Identifier ] */
                result = teep_set_any_array(message, item, QCBOR_TYPE_ANY, &query_response->unneeded_tc_list);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_response->contains |= TEEP_MESSAGE_CONTAINS_UNNEEDED_TC_LIST;
                break;
            case TEEP_OPTIONS_KEY_EXT_LIST:
                result = teep_set_any_array(message, item, QCBOR_TYPE_UINT64, &query_response->ext_list);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_response->contains |= TEEP_MESSAGE_CONTAINS_EXT_LIST;
                break;
            default:
                return TEEP_ERR_UNEXPECTED_ERROR;
        }
    }
    return TEEP_SUCCESS;
}

teep_err_t teep_set_update(QCBORDecodeContext *message,
                        QCBORItem *item,
                        teep_update_t *teep_update) {
    teep_err_t result = TEEP_SUCCESS;

    teep_update->token.ptr = NULL;
    teep_update->token.len = 0;
    INITIALIZE_TEEP_ARRAY(teep_update->unneeded_tc_list);
    INITIALIZE_TEEP_ARRAY(teep_update->manifest_list);

    result = teep_qcbor_get_next(message, item, QCBOR_TYPE_MAP);
    if (result != TEEP_SUCCESS) {
        return result;
    }
    size_t map_count = item->val.uCount;
    for (size_t i = 0; i < map_count; i++) {
        result = teep_qcbor_get_next(message, item, QCBOR_TYPE_ANY);
        if (result != TEEP_SUCCESS) {
            return result;
        }
        switch (item->label.uint64) {
            case TEEP_OPTIONS_KEY_TOKEN:
                result = teep_set_byte_string(QCBOR_TYPE_BYTE_STRING, item, &teep_update->token);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_update->contains |= TEEP_MESSAGE_CONTAINS_TOKEN;
                break;
            case TEEP_OPTIONS_KEY_UNNEEDED_TC_LIST:
                result = teep_set_any_array(message, item, QCBOR_TYPE_ANY, &teep_update->unneeded_tc_list);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_update->contains |= TEEP_MESSAGE_CONTAINS_UNNEEDED_TC_LIST;
                break;
            case TEEP_OPTIONS_KEY_MANIFEST_LIST:
                result = teep_set_any_array(message, item, QCBOR_TYPE_BYTE_STRING, &teep_update->manifest_list);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_update->contains |= TEEP_MESSAGE_CONTAINS_MANIFEST_LIST;
                break;
            default:
                return TEEP_ERR_UNEXPECTED_ERROR;
        }
    }
    return TEEP_SUCCESS;
}

teep_err_t teep_set_success(QCBORDecodeContext *message,
                         QCBORItem *item,
                         teep_success_t *teep_success) {
    teep_err_t result = TEEP_SUCCESS;

    INITIALIZE_TEEP_BUF(teep_success->msg);
    INITIALIZE_TEEP_ARRAY(teep_success->suit_reports);

    result = teep_qcbor_get_next(message, item, QCBOR_TYPE_MAP);
    if (result != TEEP_SUCCESS) {
        return result;
    }
    size_t map_count = item->val.uCount;
    for (size_t i = 0; i < map_count; i++) {
        result = teep_qcbor_get_next(message, item, QCBOR_TYPE_ANY);
        if (result != TEEP_SUCCESS) {
            return result;
        }
        switch (item->label.uint64) {
            case TEEP_OPTIONS_KEY_TOKEN:
                result = teep_set_byte_string(QCBOR_TYPE_BYTE_STRING, item, &teep_success->token);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_success->contains |= TEEP_MESSAGE_CONTAINS_TOKEN;
                break;
            case TEEP_OPTIONS_KEY_MSG:
                result = teep_set_byte_string(QCBOR_TYPE_TEXT_STRING, item, &teep_success->msg);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_success->contains |= TEEP_MESSAGE_CONTAINS_MSG;
                break;
            case TEEP_OPTIONS_KEY_SUIT_REPORTS:
                result = teep_set_any_array(message, item, QCBOR_TYPE_ANY, &teep_success->suit_reports);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_success->contains |= TEEP_MESSAGE_CONTAINS_SUIT_REPORTS;
                break;
            default:
                return TEEP_ERR_UNEXPECTED_ERROR;
        }
    }
    return TEEP_SUCCESS;
}

teep_err_t teep_set_error(QCBORDecodeContext *message,
                       QCBORItem *item,
                       teep_error_t *teep_error) {
    teep_err_t result = TEEP_SUCCESS;
    INITIALIZE_TEEP_BUF(teep_error->err_msg);
    INITIALIZE_TEEP_ARRAY(teep_error->supported_cipher_suites);
    INITIALIZE_TEEP_ARRAY(teep_error->versions);
    INITIALIZE_TEEP_ARRAY(teep_error->suit_reports);
    teep_error->err_code = TEEP_ERR_CODE_INVALID;

    result = teep_qcbor_get_next(message, item, QCBOR_TYPE_MAP);
    if (result != TEEP_SUCCESS) {
        return result;
    }
    size_t map_count = item->val.uCount;
    for (size_t i = 0; i < map_count; i++) {
        result = teep_qcbor_get_next(message, item, QCBOR_TYPE_ANY);
        if (result != TEEP_SUCCESS) {
            return result;
        }
        switch (item->label.uint64) {
            case TEEP_OPTIONS_KEY_TOKEN:
                result = teep_set_byte_string(QCBOR_TYPE_BYTE_STRING, item, &teep_error->token);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_error->contains |= TEEP_MESSAGE_CONTAINS_TOKEN;
                break;
            case TEEP_OPTIONS_KEY_ERR_MSG:
                result = teep_set_byte_string(QCBOR_TYPE_TEXT_STRING, item, &teep_error->err_msg);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_error->contains |= TEEP_MESSAGE_CONTAINS_ERR_MSG;
                break;
            case TEEP_OPTIONS_KEY_SUPPORTED_CIPHER_SUITES:
                result = teep_set_ciphersuite_array(message, item, &teep_error->supported_cipher_suites);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_error->contains |= TEEP_MESSAGE_CONTAINS_SUPPORTED_CIPHER_SUITES;
                break;
            case TEEP_OPTIONS_KEY_VERSION:
                result = teep_set_any_array(message, item, QCBOR_TYPE_UINT32, &teep_error->versions);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_error->contains |= TEEP_MESSAGE_CONTAINS_VERSION;
                break;
            case TEEP_OPTIONS_KEY_SUIT_REPORTS:
                result = teep_set_any_array(message, item, QCBOR_TYPE_ANY, &teep_error->suit_reports);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_error->contains |= TEEP_MESSAGE_CONTAINS_SUIT_REPORTS;
                break;
            default:
                return TEEP_ERR_UNEXPECTED_ERROR;
        }
    }

    result = teep_qcbor_get_next_uint64(message, item);
    if (result != TEEP_SUCCESS) {
        return result;
    }
    teep_error->err_code = item->val.uint64;
    return TEEP_SUCCESS;
}

teep_err_t teep_set_message_from_bytes(const uint8_t *buf, const size_t len, teep_message_t *msg) {
    teep_err_t result = TEEP_SUCCESS;
    QCBORDecodeContext decode_context;
    QCBORItem item;

    // decode cbor : get message type.
    QCBORDecode_Init(&decode_context,
                     (UsefulBufC){buf, len},
                     QCBOR_DECODE_MODE_NORMAL);
    msg->teep_message.type = teep_get_message_type(&decode_context);
    msg->teep_message.contains = TEEP_MESSAGE_CONTAINS_TYPE;
    switch (msg->teep_message.type) {
        case TEEP_TYPE_QUERY_REQUEST:
            result = teep_set_query_request(&decode_context, &item, &msg->query_request);
            break;
        case TEEP_TYPE_QUERY_RESPONSE:
            result = teep_set_query_response(&decode_context, &item, &msg->query_response);
            break;
        case TEEP_TYPE_UPDATE:
            result = teep_set_update(&decode_context, &item, &msg->teep_update);
            break;
        case TEEP_TYPE_TEEP_SUCCESS:
            result = teep_set_success(&decode_context, &item, &msg->teep_success);
            break;
        case TEEP_TYPE_TEEP_ERROR:
            result = teep_set_error(&decode_context, &item, &msg->teep_error);
            break;
        default:
            return TEEP_ERR_INVALID_MESSAGE_TYPE;
            break;
    }

    QCBORDecode_Finish(&decode_context);
    return result;
}
