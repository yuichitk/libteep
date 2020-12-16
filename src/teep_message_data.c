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

bool teep_qcbor_get_next(QCBORDecodeContext *message,
                    QCBORItem *item,
                    QCBORError *error,
                    uint8_t data_type) {
    if ((*error = QCBORDecode_GetNext(message, item))) {
        teep_debug_print(message, item, error, "teep_qcbor_get_next", QCBOR_TYPE_ANY);
        return false;
    }
    if (data_type != QCBOR_TYPE_ANY && item->uDataType != data_type) {
        teep_debug_print(message, item, error, "teep_qcbor_get_next", data_type);
        return false;
    }
    return true;
}

bool teep_qcbor_get_next_uint(QCBORDecodeContext *message,
                         QCBORItem *item,
                         QCBORError *error) {
    if (!teep_qcbor_get_next(message, item, error, QCBOR_TYPE_ANY)) {
        teep_debug_print(message, item, error, "teep_qcbor_get_next_uint", QCBOR_TYPE_UINT64);
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

bool teep_qcbor_skip_any(QCBORDecodeContext *message,
                    QCBORItem *item,
                    QCBORError *error);

bool teep_qcbor_skip_array_and_map(QCBORDecodeContext *message,
                            QCBORItem *item,
                            QCBORError *error) {
    if (item->uDataType != QCBOR_TYPE_ARRAY && item->uDataType != QCBOR_TYPE_MAP) {
        return false;
    }
    size_t array_size = item->val.uCount;
    for (size_t i = 0; i < array_size; i++) {
        if (!teep_qcbor_get_next(message, item, error, QCBOR_TYPE_ANY)) {
            return false;
        }
        if (!teep_qcbor_skip_any(message, item, error)) {
            return false;
        }
    }
    return true;
}

bool teep_qcbor_skip_any(QCBORDecodeContext *message,
                    QCBORItem *item,
                    QCBORError *error) {
    switch (item->uDataType) {
        case QCBOR_TYPE_ARRAY:
        case QCBOR_TYPE_MAP:
            if (!teep_qcbor_skip_array_and_map(message, item, error)) {
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
bool teep_qcbor_get_next_uint64(QCBORDecodeContext *message,
                    QCBORItem *item,
                    QCBORError *error) {
    if (!teep_qcbor_get_next(message, item, error, QCBOR_TYPE_ANY)) {
        return false;
    }
    return teep_qcbor_value_is_uint64(item);
}

uint64_t teep_get_message_type(QCBORDecodeContext *message) {
    QCBORItem  item;
    QCBORError error;
    if (!teep_qcbor_get_next(message, &item, &error, QCBOR_TYPE_ARRAY)) {
        return TEEP_TYPE_INVALID;
    }
    if (!teep_qcbor_get_next_uint64(message, &item, &error)) {
        return TEEP_TYPE_INVALID;
    }
    return item.val.uint64;
}

size_t teep_qcbor_calc_rollback(QCBORItem *item) {
    uint8_t type = item->uDataType;
    if (item->uDataType == QCBOR_TYPE_INT64 && teep_qcbor_value_is_uint64(item)) {
        type = QCBOR_TYPE_UINT64;
    }

    switch (type) {
        case QCBOR_TYPE_UINT64:
            if (item->val.uint64 < 23) {
                return 1;
            }
            else if (item->val.uint64 < UINT8_MAX) {
                return 2;
            }
            else if (item->val.uint64 < UINT16_MAX) {
                return 3;
            }
            else if (item->val.uint64 < UINT32_MAX) {
                return 4;
            }
            return 5;
        case QCBOR_TYPE_INT64:
            if (item->val.int64 > -25) {
                return 1;
            }
            else if (item->val.int64 > -1 - UINT8_MAX) {
                return 2;
            }
            else if (item->val.int64 > -1 - UINT16_MAX) {
                return 3;
            }
            else if (item->val.int64 > -1 - UINT32_MAX) {
                return 4;
            }
            return 5;
        case QCBOR_TYPE_BYTE_STRING:
        case QCBOR_TYPE_TEXT_STRING:
            if (item->val.string.len < 24) {
                return 1;
            }
            else if (item->val.string.len < UINT8_MAX) {
                return 2;
            }
            else if (item->val.string.len < UINT16_MAX) {
                return 3;
            }
            else if (item->val.string.len < UINT32_MAX) {
                return 4;
            }
            return 5;
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
                return 4;
            }
            return 5;
    }
    return 0;
}

/*
 * set ptr and skip out of teep message cbor part such as suit-report
 */
int32_t teep_set_out_of_teep_buf(QCBORDecodeContext *message,
                            QCBORItem *item,
                            QCBORError *error,
                            teep_buf_t *teep_buf) {
    /* roll back the cursor locally,
     * because QCBORDecode_GetNext() to Map's key-value set the cursor after the value end.
     * e.g. Map { unsigned(1) : array(2) [ unsigned(2), unsigned(2) ] }
                                                                    ^
                                                                    |
     */
    uint8_t *buf;
    size_t rollback = teep_qcbor_calc_rollback(item);
    if (rollback == 0) {
        return TEEP_UNEXPECTED_ERROR;
    }
    size_t cursor = UsefulInputBuf_Tell(&message->InBuf);
    buf = (uint8_t *)message->InBuf.UB.ptr + (cursor - rollback);
    if (!teep_qcbor_skip_any(message, item, error)) {
        return TEEP_UNEXPECTED_ERROR;
    }
    teep_buf->ptr = buf;
    teep_buf->len = UsefulInputBuf_Tell(&message->InBuf) - (cursor - rollback);

    return TEEP_SUCCESS;
}

int32_t teep_set_any_array(QCBORDecodeContext *message,
                           QCBORItem *item,
                           QCBORError *error,
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
        printf("\nteep_set_any_array(%d) : Error! uDataType = %d\n", targetDataType, item->uDataType);
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    if (item->val.uCount > TEEP_MAX_ARRAY_LENGTH) {
        return TEEP_NO_MEMORY;
    }
    size_t array_size = item->val.uCount;
    for (size_t j = 0; j < array_size; j++) {
        if (!teep_qcbor_get_next(message, item, error, QCBOR_TYPE_ANY)) {
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        switch(targetDataType) {
            case QCBOR_TYPE_UINT64:
                if (!teep_qcbor_value_is_uint64(item)) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                ptr.u64->items[j] = item->val.uint64;
                ptr.u64->len = j + 1;
                break;
            case QCBOR_TYPE_UINT32:
                /* extended type */
                if (!teep_qcbor_value_is_uint32(item)) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                ptr.u32->items[j] = (uint32_t)item->val.uint64;
                ptr.u32->len = j + 1;
                break;
            case QCBOR_TYPE_BYTE_STRING:
                if (item->uDataType != QCBOR_TYPE_BYTE_STRING) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                ptr.b->items[j].ptr = item->val.string.ptr;
                ptr.b->items[j].len = item->val.string.len;
                ptr.b->len = j + 1;
                break;
            default:
                break;
        }
    }
    return TEEP_SUCCESS;
}

int32_t teep_set_byte_string(uint8_t data_type,
                             QCBORItem *item,
                             teep_buf_t *buf) {
    if (item->uDataType != data_type) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    buf->ptr = item->val.string.ptr;
    buf->len = item->val.string.len;
    return TEEP_SUCCESS;
}

int32_t teep_set_tc_info_array(QCBORDecodeContext *message,
                               QCBORItem *item,
                               QCBORError *error,
                               teep_tc_info_array_t *tc_info_arr) {
    int32_t result = TEEP_SUCCESS;

    INITIALIZE_TEEP_ARRAY_PTR(tc_info_arr);

    if (item->uDataType != QCBOR_TYPE_ARRAY) {
        printf("\nteep_set_tc_info_array : Error! uDataType = %d\n", item->uDataType);
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    if (item->val.uCount > TEEP_MAX_ARRAY_LENGTH) {
        return TEEP_NO_MEMORY;
    }
    tc_info_arr->len = item->val.uCount;
    for (size_t i = 0; i < tc_info_arr->len; i++) {
        if (!teep_qcbor_get_next(message, item, error, QCBOR_TYPE_MAP)) {
            tc_info_arr->len = i;
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        size_t map_count = item->val.uCount;
        for (size_t j = 0; j < map_count; j++) {
            if (!teep_qcbor_get_next(message, item, error, QCBOR_TYPE_ANY)) {
                return TEEP_UNEXPECTED_ERROR;
            }
            tc_info_arr->items[i].contains = 0UL;
            tc_info_arr->items[i].tc_manifest_sequence_number = TEEP_SUIT_MANIFEST_SEQUENCE_NUMBER_INVALID;
            switch (item->label.uint64) {
                case TEEP_OPTIONS_KEY_COMPONENT_ID:
                    /* SUIT_Component_Identifier = [ +bstr ] (currently) */
                    if (item->uDataType != QCBOR_TYPE_ARRAY) {
                        result = TEEP_INVALID_TYPE_OF_ARGUMENT;
                        break;
                    }
                    result = teep_set_out_of_teep_buf(message, item, error, &tc_info_arr->items[i].component_id);
                    if (result != TEEP_SUCCESS) {
                        break;
                    }
                    tc_info_arr->items[i].contains |= TEEP_MESSAGE_CONTAINS_COMPONENT_ID;
                    break;
                case TEEP_OPTIONS_KEY_TC_MANIFEST_SEQUENCE_NUMBER:
                    if (!teep_qcbor_value_is_uint64(item)) {
                        result = TEEP_INVALID_TYPE_OF_ARGUMENT;
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

int32_t teep_set_requested_tc_info_array(QCBORDecodeContext *message,
                                         QCBORItem *item,
                                         QCBORError *error,
                                         teep_requested_tc_info_array_t *tc_info_arr) {
    int32_t result = TEEP_SUCCESS;

    INITIALIZE_TEEP_ARRAY_PTR(tc_info_arr);

    if (item->uDataType != QCBOR_TYPE_ARRAY) {
        printf("\nteep_set_tc_info_array : Error! uDataType = %d\n", item->uDataType);
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    if (item->val.uCount > TEEP_MAX_ARRAY_LENGTH) {
        return TEEP_NO_MEMORY;
    }
    tc_info_arr->len = item->val.uCount;
    for (size_t i = 0; i < tc_info_arr->len; i++) {
        if (!teep_qcbor_get_next(message, item, error, QCBOR_TYPE_MAP)) {
            tc_info_arr->len = i;
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        size_t map_count = item->val.uCount;
        for (size_t j = 0; j < map_count; j++) {
            if (!teep_qcbor_get_next(message, item, error, QCBOR_TYPE_ANY)) {
                return TEEP_INVALID_TYPE_OF_ARGUMENT;
            }
            tc_info_arr->items[i].contains = 0UL;
            tc_info_arr->items[i].tc_manifest_sequence_number = TEEP_SUIT_MANIFEST_SEQUENCE_NUMBER_INVALID;
            switch (item->label.uint64) {
                case TEEP_OPTIONS_KEY_COMPONENT_ID:
                    /* SUIT_Component_Identifier = [ +bstr ] (currently) */
                    result = teep_set_out_of_teep_buf(message, item, error, &tc_info_arr->items[i].component_id);
                    if (result != TEEP_SUCCESS) {
                        break;
                    }
                    tc_info_arr->items[i].contains |= TEEP_MESSAGE_CONTAINS_COMPONENT_ID;
                    break;
                case TEEP_OPTIONS_KEY_TC_MANIFEST_SEQUENCE_NUMBER:
                    if (!teep_qcbor_value_is_uint64(item)) {
                        result = TEEP_INVALID_TYPE_OF_ARGUMENT;
                        break;
                    }
                    tc_info_arr->items[i].tc_manifest_sequence_number = item->val.uint64;
                    tc_info_arr->items[i].contains |= TEEP_MESSAGE_CONTAINS_TC_MANIFEST_SEQUENCE_NUMBER;
                    break;
                case TEEP_OPTIONS_KEY_HAVE_BINARY:
                    if (item->uDataType != QCBOR_TYPE_TRUE && item->uDataType != QCBOR_TYPE_FALSE) {
                        result = TEEP_INVALID_TYPE_OF_ARGUMENT;
                        break;
                    }
                    tc_info_arr->items[i].have_binary = (bool)item->val.uint64;
                    tc_info_arr->items[i].contains |= TEEP_MESSAGE_CONTAINS_HAVE_BINARY;
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

int32_t teep_set_query_request(QCBORDecodeContext *message,
                               QCBORItem *item,
                               QCBORError *error,
                               teep_query_request_t *query_request) {
    int32_t result = TEEP_SUCCESS;

    query_request->contains = 0UL;
    query_request->type = TEEP_TYPE_QUERY_REQUEST;
    query_request->token = 0U;
    INITIALIZE_TEEP_ARRAY(query_request->supported_cipher_suites);
    INITIALIZE_TEEP_BUF(query_request->challenge);
    INITIALIZE_TEEP_ARRAY(query_request->versions);
    INITIALIZE_TEEP_BUF(query_request->ocsp_data);
    query_request->data_item_requested = TEEP_DATA_ITEM_INVALID;

    query_request->type = teep_get_message_type(message);
    if (query_request->type != TEEP_TYPE_QUERY_REQUEST) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    query_request->contains |= TEEP_MESSAGE_CONTAINS_TYPE;

    if (!teep_qcbor_get_next(message, item, error, QCBOR_TYPE_MAP)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    size_t map_count = item->val.uCount;
    for (size_t i = 0; i < map_count; i++) {
        if (!teep_qcbor_get_next(message, item, error, QCBOR_TYPE_ANY)) {
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        switch (item->label.uint64) {
            case TEEP_OPTIONS_KEY_TOKEN:
                if (!teep_qcbor_value_is_uint32(item)) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                query_request->token = (uint32_t)item->val.uint64;
                query_request->contains |= TEEP_MESSAGE_CONTAINS_TOKEN;
                break;
            case TEEP_OPTIONS_KEY_SUPPORTED_CIPHER_SUITES:
                result = teep_set_any_array(message, item, error, QCBOR_TYPE_UINT64, &query_request->supported_cipher_suites);
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
                result = teep_set_any_array(message, item, error, QCBOR_TYPE_UINT32, &query_request->versions);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_request->contains |= TEEP_MESSAGE_CONTAINS_VERSION;
                break;
            case TEEP_OPTIONS_KEY_OCSP_DATA:
                result = teep_set_byte_string(QCBOR_TYPE_BYTE_STRING, item, &query_request->ocsp_data);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_request->contains |= TEEP_MESSAGE_CONTAINS_OCSP_DATA;
                break;
            default:
                break;
        }
    }
    if (!teep_qcbor_get_next_uint64(message, item, error)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    query_request->data_item_requested = item->val.uint64;
    return TEEP_SUCCESS;
}

int32_t teep_set_query_response(QCBORDecodeContext *message,
                                QCBORItem *item,
                                QCBORError *error,
                                teep_query_response_t *query_response) {
    int32_t result = TEEP_SUCCESS;

    query_response->contains = 0UL;
    query_response->type = TEEP_TYPE_INVALID;
    query_response->token = 0U;
    query_response->selected_cipher_suite = TEEP_SUITE_INVALID;
    query_response->selected_version = 0U;
    INITIALIZE_TEEP_BUF(query_response->evidence_format);
    INITIALIZE_TEEP_BUF(query_response->evidence);
    INITIALIZE_TEEP_ARRAY(query_response->tc_list);
    INITIALIZE_TEEP_ARRAY(query_response->requested_tc_list);
    INITIALIZE_TEEP_ARRAY(query_response->unneeded_tc_list);
    INITIALIZE_TEEP_ARRAY(query_response->ext_list);

    query_response->type = teep_get_message_type(message);
    if (query_response->type != TEEP_TYPE_QUERY_RESPONSE) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    query_response->contains |= TEEP_MESSAGE_CONTAINS_TYPE;

    if (!teep_qcbor_get_next(message, item, error, QCBOR_TYPE_MAP)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    size_t map_count = item->val.uCount;
    for (size_t i = 0; i < map_count; i++) {
        if (!teep_qcbor_get_next(message, item, error, QCBOR_TYPE_ANY)) {
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        switch (item->label.uint64) {
            case TEEP_OPTIONS_KEY_TOKEN:
                if (!teep_qcbor_value_is_uint32(item)) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                query_response->token = (uint32_t)item->val.uint64;
                query_response->contains |= TEEP_MESSAGE_CONTAINS_TOKEN;
                break;
            case TEEP_OPTIONS_KEY_SELECTED_CIPHER_SUITE:
                if (!teep_qcbor_value_is_uint64(item)) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                query_response->selected_cipher_suite = item->val.uint64;
                query_response->contains |= TEEP_MESSAGE_CONTAINS_SELECTED_CIPHER_SUITE;
                break;
            case TEEP_OPTIONS_KEY_SELECTED_VERSION:
                if (!teep_qcbor_value_is_uint32(item)) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
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
                result = teep_set_tc_info_array(message, item, error, &query_response->tc_list);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_response->contains |= TEEP_MESSAGE_CONTAINS_TC_LIST;
                break;
            case TEEP_OPTIONS_KEY_REQUESTED_TC_LIST:
                result = teep_set_requested_tc_info_array(message, item, error, &query_response->requested_tc_list);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_response->contains |= TEEP_MESSAGE_CONTAINS_REQUESTED_TC_LIST;
                break;
            case TEEP_OPTIONS_KEY_UNNEEDED_TC_LIST:
                /* [ + SUIT_Component_Identifier ] */
                result = teep_set_any_array(message, item, error, QCBOR_TYPE_ANY, &query_response->unneeded_tc_list);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_response->contains |= TEEP_MESSAGE_CONTAINS_UNNEEDED_TC_LIST;
            case TEEP_OPTIONS_KEY_EXT_LIST:
                result = teep_set_any_array(message, item, error, QCBOR_TYPE_UINT64, &query_response->ext_list);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_response->contains |= TEEP_MESSAGE_CONTAINS_EXT_LIST;
                break;
            default:
                break;
        }
    }
    return TEEP_SUCCESS;
}

int32_t teep_set_update(QCBORDecodeContext *message,
                        QCBORItem *item,
                        QCBORError *error,
                        teep_update_t *teep_update) {
    int32_t result = TEEP_SUCCESS;

    teep_update->contains = 0UL;
    teep_update->type = TEEP_TYPE_INVALID;
    teep_update->token = 0U;
    INITIALIZE_TEEP_ARRAY(teep_update->tc_list);
    INITIALIZE_TEEP_ARRAY(teep_update->manifest_list);

    teep_update->type = teep_get_message_type(message);
    if (teep_update->type != TEEP_TYPE_UPDATE) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    teep_update->contains |= TEEP_MESSAGE_CONTAINS_TYPE;

    if (!teep_qcbor_get_next(message, item, error, QCBOR_TYPE_MAP)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    size_t map_count = item->val.uCount;
    for (size_t i = 0; i < map_count; i++) {
        if (!teep_qcbor_get_next(message, item, error, QCBOR_TYPE_ANY)) {
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        switch (item->label.uint64) {
            case TEEP_OPTIONS_KEY_TOKEN:
                if (!teep_qcbor_value_is_uint32(item)) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                teep_update->token = (uint32_t)item->val.uint64;
                teep_update->contains |= TEEP_MESSAGE_CONTAINS_TOKEN;
                break;
            case TEEP_OPTIONS_KEY_TC_LIST:
                result = teep_set_any_array(message, item, error, QCBOR_TYPE_ANY, &teep_update->tc_list);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_update->contains |= TEEP_MESSAGE_CONTAINS_TC_LIST;
                break;
            case TEEP_OPTIONS_KEY_MANIFEST_LIST:
                result = teep_set_any_array(message, item, error, QCBOR_TYPE_BYTE_STRING, &teep_update->manifest_list);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_update->contains |= TEEP_MESSAGE_CONTAINS_MANIFEST_LIST;
                break;
            default:
                break;
        }
    }
    return TEEP_SUCCESS;
}

int32_t teep_set_success(QCBORDecodeContext *message,
                         QCBORItem *item,
                         QCBORError *error,
                         teep_success_t *teep_success) {
    int32_t result = TEEP_SUCCESS;

    teep_success->contains = 0UL;
    teep_success->type = TEEP_TYPE_TEEP_SUCCESS;
    teep_success->type = 0U;
    INITIALIZE_TEEP_BUF(teep_success->msg);
    INITIALIZE_TEEP_BUF(teep_success->suit_reports);

    teep_success->type = teep_get_message_type(message);
    if (teep_success->type != TEEP_TYPE_TEEP_SUCCESS) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    teep_success->contains |= TEEP_MESSAGE_CONTAINS_TYPE;

    if (!teep_qcbor_get_next(message, item, error, QCBOR_TYPE_MAP)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    size_t map_count = item->val.uCount;
    for (size_t i = 0; i < map_count; i++) {
        if (!teep_qcbor_get_next(message, item, error, QCBOR_TYPE_ANY)) {
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        switch (item->label.uint64) {
            case TEEP_OPTIONS_KEY_TOKEN:
                if (!teep_qcbor_value_is_uint32(item)) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                teep_success->token = (uint32_t)item->val.uint64;
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
                result = teep_set_any_array(message, item, error, QCBOR_TYPE_ANY, &teep_success->suit_reports);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_success->contains |= TEEP_MESSAGE_CONTAINS_SUIT_REPORTS;
                break;
            default:
                break;
        }
    }
    return TEEP_SUCCESS;
}

int32_t teep_set_error(QCBORDecodeContext *message,
                       QCBORItem *item,
                       QCBORError *error,
                       teep_error_t *teep_error) {
    int32_t result = TEEP_SUCCESS;

    teep_error->contains = 0UL;
    teep_error->type = TEEP_TYPE_TEEP_ERROR;
    teep_error->token = 0U;
    INITIALIZE_TEEP_BUF(teep_error->err_msg);
    INITIALIZE_TEEP_ARRAY(teep_error->supported_cipher_suites);
    INITIALIZE_TEEP_ARRAY(teep_error->versions);
    INITIALIZE_TEEP_BUF(teep_error->suit_reports);
    teep_error->err_code = TEEP_ERR_CODE_INVALID;

    teep_error->type = teep_get_message_type(message);
    if (teep_error->type != TEEP_TYPE_TEEP_ERROR) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    teep_error->contains |= TEEP_MESSAGE_CONTAINS_TYPE;

    if (!teep_qcbor_get_next(message, item, error, QCBOR_TYPE_MAP)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    size_t map_count = item->val.uCount;
    for (size_t i = 0; i < map_count; i++) {
        if (!teep_qcbor_get_next(message, item, error, QCBOR_TYPE_ANY)) {
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        switch (item->label.uint64) {
            case TEEP_OPTIONS_KEY_TOKEN:
                if (!teep_qcbor_value_is_uint32(item)) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                teep_error->token = (uint32_t)item->val.uint64;
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
                if (item->uDataType != QCBOR_TYPE_ARRAY) {
                    printf("\nteep_set_error : Error! uDataType = %d\n", item->uDataType);
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                if (item->val.uCount > TEEP_MAX_ARRAY_LENGTH) {
                    return TEEP_NO_MEMORY;
                }
                teep_error->supported_cipher_suites.len = item->val.uCount;
                for (size_t j = 0; j < teep_error->supported_cipher_suites.len; j++) {
                    if (!teep_qcbor_get_next_uint64(message, item, error)) {
                        teep_error->supported_cipher_suites.len = j;
                        result = TEEP_INVALID_TYPE_OF_ARGUMENT;
                        break;
                    }
                    teep_error->supported_cipher_suites.items[j] = item->val.uint64;
                }
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_error->contains |= TEEP_MESSAGE_CONTAINS_SUPPORTED_CIPHER_SUITES;
                break;
            case TEEP_OPTIONS_KEY_VERSION:
                result = teep_set_any_array(message, item, error, QCBOR_TYPE_UINT32, &teep_error->versions);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_error->contains |= TEEP_MESSAGE_CONTAINS_VERSION;
                break;
            case TEEP_OPTIONS_KEY_SUIT_REPORTS:
                result = teep_set_byte_string(QCBOR_TYPE_BYTE_STRING, item, &teep_error->suit_reports);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_error->contains |= TEEP_MESSAGE_CONTAINS_SUIT_REPORTS;
                break;
            default:
                break;
        }
    }

    if (!teep_qcbor_get_next_uint64(message, item, error)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    teep_error->err_code = item->val.uint64;
    teep_error->contains |= TEEP_MESSAGE_CONTAINS_ERR_CODE;
    return TEEP_SUCCESS;
}

int32_t teep_set_message_from_bytes(const uint8_t *buf, const size_t len, teep_message_t *msg) {
    int32_t result = TEEP_SUCCESS;
    QCBORDecodeContext  decode_context;
    QCBORItem  item;
    QCBORError error;

    // decode cbor : get message type.
    QCBORDecode_Init(&decode_context,
                     (UsefulBufC){buf, len},
                     QCBOR_DECODE_MODE_NORMAL);
    uint64_t message_type = teep_get_message_type(&decode_context);
    QCBORDecode_Finish(&decode_context);

    // decode cbor : get message.
    QCBORDecode_Init(&decode_context,
                     (UsefulBufC){buf, len},
                     QCBOR_DECODE_MODE_NORMAL);
    switch (message_type) {
        case TEEP_TYPE_QUERY_REQUEST:
            result = teep_set_query_request(&decode_context, &item, &error, &msg->query_request);
            break;
        case TEEP_TYPE_QUERY_RESPONSE:
            result = teep_set_query_response(&decode_context, &item, &error, &msg->query_response);
            break;
        case TEEP_TYPE_UPDATE:
            result = teep_set_update(&decode_context, &item, &error, &msg->teep_update);
            break;
        case TEEP_TYPE_TEEP_SUCCESS:
            result = teep_set_success(&decode_context, &item, &error, &msg->teep_success);
            break;
        case TEEP_TYPE_TEEP_ERROR:
            result = teep_set_error(&decode_context, &item, &error, &msg->teep_error);
            break;
        default:
            teep_print_debug_string("print_teep_message : Undefined value.\n");
            break;
    }

    QCBORDecode_Finish(&decode_context);
    return result;
}
