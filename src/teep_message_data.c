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

bool qcbor_get_next_nockeck(QCBORDecodeContext *message,
                            QCBORItem *item,
                            QCBORError *error) {
    if ((*error = QCBORDecode_GetNext(message, item))) {
        printf("\ncbor_get_next : Error! nCBORError = %d\n", (int32_t)*error);
        return false;
    }
    return true;
}

bool qcbor_get_next(QCBORDecodeContext *message,
                    uint8_t data_type,
                    QCBORItem *item,
                    QCBORError *error) {
    if (!qcbor_get_next_nockeck(message, item, error)) {
        return false;
    }
    if (item->uDataType != data_type) {
        printf("\ncbor_get_next : Error! uDataType = %d (expecting %d)\n", item->uDataType, data_type);
        return false;
    }
    return true;
}

bool qcbor_skip_any(QCBORDecodeContext *message,
                    QCBORItem *item,
                    QCBORError *error);

bool qcbor_skip_array_and_map(QCBORDecodeContext *message,
                        QCBORItem *item,
                        QCBORError *error) {
    if (item->uDataType != QCBOR_TYPE_ARRAY && item->uDataType != QCBOR_TYPE_MAP) {
        return false;
    }
    for (size_t i = 0; i < item->val.uCount; i++) {
        if (qcbor_skip_any(message, item, error)) {
            return false;
        }
    }
    return true;
}

bool qcbor_skip_any(QCBORDecodeContext *message,
                    QCBORItem *item,
                    QCBORError *error) {
    if (!qcbor_get_next_nockeck(message, item, error)) {
        return false;
    }
    switch (item->uDataType) {
        case QCBOR_TYPE_ARRAY:
        case QCBOR_TYPE_MAP:
            if (qcbor_skip_array_and_map(message, item, error)) {
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

bool qcbor_value_is_uint64(QCBORItem *item) {
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

bool qcbor_value_is_uint32(QCBORItem *item) {
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
bool qcbor_get_next_uint64(QCBORDecodeContext *message,
                    QCBORItem *item,
                    QCBORError *error) {
    if (!qcbor_get_next_nockeck(message, item, error)) {
        return false;
    }
    return qcbor_value_is_uint64(item);
}

uint64_t get_teep_message_type(QCBORDecodeContext *message) {
    QCBORItem  item;
    QCBORError error;
    if (!qcbor_get_next(message, QCBOR_TYPE_ARRAY, &item, &error)) {
        return TEEP_TYPE_INVALID;
    }
    if (!qcbor_get_next_uint64(message, &item, &error)) {
        return TEEP_TYPE_INVALID;
    }
    return item.val.uint64;
}

/*
 * set ptr and skip out of teep message cbor part such as suit-report
 */
int32_t set_out_of_teep_buf(QCBORDecodeContext *message,
                            QCBORItem *item,
                            QCBORError *error,
                            teep_buf_t *teep_buf) {
    uint8_t *buf = (uint8_t *)message->InBuf.UB.ptr + UsefulInputBuf_Tell(&message->InBuf);
    if (qcbor_skip_any(message, item, error)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    teep_buf->ptr = buf;
    teep_buf->len = (uint8_t *)message->InBuf.UB.ptr + UsefulInputBuf_Tell(&message->InBuf) - buf;

    return TEEP_SUCCESS;
}

int32_t set_teep_any_array(QCBORDecodeContext *message,
                           QCBORItem *item,
                           QCBORError *error,
                           uint8_t targetDataType,
                           void *teep_any_array) {
    int32_t result = TEEP_SUCCESS;
    union {
        void *v;
        teep_uint64_array_t *u64;
        teep_uint32_array_t *u32;
        teep_buf_array_t *b;
    } ptr;
    ptr.v = teep_any_array;

    if (item->uDataType != QCBOR_TYPE_ARRAY) {
        printf("\nset_teep_any_array(%d) : Error! uDataType = %d\n", targetDataType, item->uDataType);
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    if (item->val.uCount > TEEP_MAX_ARRAY_LENGTH) {
        return TEEP_NO_MEMORY;
    }
    for (size_t j = 0; j < item->val.uCount; j++) {
        if (!qcbor_get_next_nockeck(message, item, error)) {
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        switch(targetDataType) {
            case QCBOR_TYPE_UINT64:
                if (!qcbor_value_is_uint64(item)) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                ptr.u64->items[j] = item->val.uint64;
                ptr.u64->len = j;
                break;
            case QCBOR_TYPE_UINT32:
                /* extended type */
                if (!qcbor_value_is_uint32(item)) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                ptr.u32->items[j] = (uint32_t)item->val.uint64;
                ptr.u32->len = j;
                break;
            case QCBOR_TYPE_BYTE_STRING:
                if (item->uDataType != QCBOR_TYPE_BYTE_STRING) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                ptr.b->items[j].ptr = item->val.string.ptr;
                ptr.b->items[j].len = item->val.string.len;
                ptr.b->len = j;
                break;
            case QCBOR_TYPE_ANY:
                /* out_of_teep type, so directly store the cbor buf ptr */
                result = set_out_of_teep_buf(message, item, error, &ptr.b->items[j]);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                ptr.b->len = j;
                break;
            default:
                break;
        }
    }
    return TEEP_SUCCESS;
}

int32_t set_teep_byte_string(uint8_t data_type,
                             QCBORItem *item,
                             teep_buf_t *buf) {
    if (item->uDataType != data_type) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    buf->ptr = item->val.string.ptr;
    buf->len = item->val.string.len;
    return TEEP_SUCCESS;
}

int32_t set_teep_tc_info_array(QCBORDecodeContext *message,
                                QCBORItem *item,
                                QCBORError *error,
                                teep_tc_info_array_t *tc_info_arr) {
    int32_t result;

    INITIALIZE_TEEP_ARRAY_PTR(tc_info_arr);

    if (item->uDataType != QCBOR_TYPE_ARRAY) {
        printf("\nset_teep_tc_info_array : Error! uDataType = %d\n", item->uDataType);
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    if (item->val.uCount > TEEP_MAX_ARRAY_LENGTH) {
        return TEEP_NO_MEMORY;
    }
    tc_info_arr->len = item->val.uCount;
    for (size_t i = 0; i < tc_info_arr->len; i++) {
        if (!qcbor_get_next(message, QCBOR_TYPE_MAP, item, error)) {
            tc_info_arr->len = i;
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        size_t map_count = item->val.uCount;
        for (size_t j = 0; j < map_count; j++) {
            if (!qcbor_get_next_nockeck(message, item, error)) {
                return TEEP_INVALID_TYPE_OF_ARGUMENT;
            }
            tc_info_arr->items[i].tc_manifest_sequence_number = TEEP_SUIT_MANIFEST_SEQUENCE_NUMBER_INVALID;
            switch (item->label.uint64) {
                case TEEP_OPTIONS_KEY_COMPONENT_ID:
                    result = set_teep_any_array(message, item, error, QCBOR_TYPE_ANY, &tc_info_arr->items[i].component_id);
                    if (result != TEEP_SUCCESS) {
                        tc_info_arr->len = i;
                        return result;
                    }
                    break;
                case TEEP_OPTIONS_KEY_TC_MANIFEST_SEQUENCE_NUMBER:
                    if (!qcbor_value_is_uint64(item)) {
                        result = TEEP_INVALID_TYPE_OF_ARGUMENT;
                        break;
                    }
                    tc_info_arr->items[i].tc_manifest_sequence_number = item->val.uint64;
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

int32_t set_teep_requested_tc_info_array(QCBORDecodeContext *message,
                                QCBORItem *item,
                                QCBORError *error,
                                teep_requested_tc_info_array_t *tc_info_arr) {
    int32_t result;

    INITIALIZE_TEEP_ARRAY_PTR(tc_info_arr);

    if (item->uDataType != QCBOR_TYPE_ARRAY) {
        printf("\nset_teep_tc_info_array : Error! uDataType = %d\n", item->uDataType);
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    if (item->val.uCount > TEEP_MAX_ARRAY_LENGTH) {
        return TEEP_NO_MEMORY;
    }
    tc_info_arr->len = item->val.uCount;
    for (size_t i = 0; i < tc_info_arr->len; i++) {
        if (!qcbor_get_next(message, QCBOR_TYPE_MAP, item, error)) {
            tc_info_arr->len = i;
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        size_t map_count = item->val.uCount;
        for (size_t j = 0; j < map_count; j++) {
            if (!qcbor_get_next_nockeck(message, item, error)) {
                return TEEP_INVALID_TYPE_OF_ARGUMENT;
            }
            tc_info_arr->items[i].tc_manifest_sequence_number = TEEP_SUIT_MANIFEST_SEQUENCE_NUMBER_INVALID;
            switch (item->label.uint64) {
                case TEEP_OPTIONS_KEY_COMPONENT_ID:
                    result = set_teep_any_array(message, item, error, QCBOR_TYPE_ANY, &tc_info_arr->items[i].component_id);
                    if (result != TEEP_SUCCESS) {
                        tc_info_arr->len = i;
                        return result;
                    }
                    break;
                case TEEP_OPTIONS_KEY_TC_MANIFEST_SEQUENCE_NUMBER:
                    if (!qcbor_value_is_uint64(item)) {
                        result = TEEP_INVALID_TYPE_OF_ARGUMENT;
                        break;
                    }
                    tc_info_arr->items[i].tc_manifest_sequence_number = item->val.uint64;
                    break;
                case TEEP_OPTIONS_KEY_HAVE_BINARY:
                    if (!qcbor_value_is_uint64(item)) {
                        result = TEEP_INVALID_TYPE_OF_ARGUMENT;
                        break;
                    }
                    tc_info_arr->items[i].have_binary = item->val.uint64;
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

int32_t set_teep_query_request(QCBORDecodeContext *message,
                               teep_query_request_t *query_request) {
    QCBORItem  item;
    QCBORError error;
    int32_t result;

    query_request->contains = TEEP_MESSAGE_CONTAINS_TYPE;
    query_request->type = TEEP_TYPE_QUERY_REQUEST;
    query_request->token = 0U;
    INITIALIZE_TEEP_ARRAY(query_request->supported_cipher_suites);
    INITIALIZE_TEEP_BUF(query_request->challenge);
    INITIALIZE_TEEP_ARRAY(query_request->versions);
    INITIALIZE_TEEP_BUF(query_request->ocsp_data);
    query_request->data_item_requested = TEEP_DATA_ITEM_INVALID;

    query_request->type = get_teep_message_type(message);
    if (query_request->type != TEEP_TYPE_QUERY_REQUEST) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }

    if (!qcbor_get_next(message, QCBOR_TYPE_MAP, &item, &error)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    uint16_t map_count = item.val.uCount;
    for (size_t i = 0; i < map_count; i++) {
        if (!qcbor_get_next_nockeck(message, &item, &error)) {
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        switch (item.label.uint64) {
            case TEEP_OPTIONS_KEY_TOKEN:
                if (!qcbor_value_is_uint64(&item)) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                query_request->contains |= TEEP_MESSAGE_CONTAINS_TOKEN;
                query_request->token = item.val.uint64;
                break;
            case TEEP_OPTIONS_KEY_SUPPORTED_CIPHER_SUITES:
                result = set_teep_any_array(message, &item, &error, QCBOR_TYPE_UINT64, &query_request->supported_cipher_suites);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_request->contains |= TEEP_MESSAGE_CONTAINS_SUPPORTED_CIPHER_SUITES;
                break;
            case TEEP_OPTIONS_KEY_CHALLENGE:
                result = set_teep_byte_string(QCBOR_TYPE_BYTE_STRING, &item, &query_request->challenge);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_request->contains |= TEEP_MESSAGE_CONTAINS_CHALLENGE;
                break;
            case TEEP_OPTIONS_KEY_VERSION:
                result = set_teep_any_array(message, &item, &error, QCBOR_TYPE_UINT32, &query_request->versions);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_request->contains |= TEEP_MESSAGE_CONTAINS_VERSION;
                break;
            case TEEP_OPTIONS_KEY_OCSP_DATA:
                result = set_teep_byte_string(QCBOR_TYPE_BYTE_STRING, &item, &query_request->ocsp_data);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_request->contains |= TEEP_MESSAGE_CONTAINS_OCSP_DATA;
                break;
            default:
                break;
        }
    }
    if (!qcbor_get_next_uint64(message, &item, &error)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    query_request->data_item_requested = item.val.uint64;
    return TEEP_SUCCESS;
}

int32_t set_teep_query_response(QCBORDecodeContext *message,
                                teep_query_response_t *query_response) {
    QCBORItem  item;
    QCBORError error;
    int32_t result;

    query_response->contains = TEEP_MESSAGE_CONTAINS_TYPE;
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

    query_response->type = get_teep_message_type(message);
    if (query_response->type != TEEP_TYPE_QUERY_RESPONSE) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }

    if (!qcbor_get_next(message, QCBOR_TYPE_MAP, &item, &error)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    size_t map_count = item.val.uCount;
    for (size_t i = 0; i < map_count; i++) {
        if (!qcbor_get_next_nockeck(message, &item, &error)) {
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        switch (item.label.uint64) {
            case TEEP_OPTIONS_KEY_TOKEN:
                if (!qcbor_value_is_uint64(&item)) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                query_response->token = item.val.uint64;
                query_response->contains |= TEEP_MESSAGE_CONTAINS_TOKEN;
                break;
            case TEEP_OPTIONS_KEY_SELECTED_CIPHER_SUITE:
                if (!qcbor_value_is_uint64(&item)) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                query_response->selected_cipher_suite = item.val.uint64;
                query_response->contains |= TEEP_MESSAGE_CONTAINS_SELECTED_CIPHER_SUITE;
                break;
            case TEEP_OPTIONS_KEY_SELECTED_VERSION:
                if (!qcbor_value_is_uint64(&item)) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                query_response->selected_version = item.val.uint64;
                query_response->contains |= TEEP_MESSAGE_CONTAINS_SELECTED_VERSION;
                break;
            case TEEP_OPTIONS_KEY_EVIDENCE_FORMAT:
                result = set_teep_byte_string(QCBOR_TYPE_TEXT_STRING, &item, &query_response->evidence_format);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_response->contains |= TEEP_MESSAGE_CONTAINS_EVIDENCE_FORMAT;
                break;
            case TEEP_OPTIONS_KEY_EVIDENCE:
                result = set_teep_byte_string(QCBOR_TYPE_BYTE_STRING, &item, &query_response->evidence);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_response->contains |= TEEP_MESSAGE_CONTAINS_EVIDENCE;
                break;
            case TEEP_OPTIONS_KEY_TC_LIST:
                result = set_teep_tc_info_array(message, &item, &error, &query_response->tc_list);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_response->contains |= TEEP_MESSAGE_CONTAINS_TC_LIST;
                break;
            case TEEP_OPTIONS_KEY_REQUESTED_TC_LIST:
                result = set_teep_requested_tc_info_array(message, &item, &error, &query_response->requested_tc_list);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                query_response->contains |= TEEP_MESSAGE_CONTAINS_REQUESTED_TC_LIST;
                break;
            case TEEP_OPTIONS_KEY_EXT_LIST:
                result = set_teep_any_array(message, &item, &error, QCBOR_TYPE_UINT64, &query_response->ext_list);
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

int32_t set_teep_update(QCBORDecodeContext *message,
                                     teep_update_t *teep_update) {
    QCBORItem  item;
    QCBORError error;
    int32_t result;

    teep_update->contains = 0UL;
    teep_update->type = TEEP_TYPE_INVALID;
    teep_update->token = 0U;
    INITIALIZE_TEEP_ARRAY(teep_update->tc_list);
    INITIALIZE_TEEP_ARRAY(teep_update->manifest_list);

    teep_update->type = get_teep_message_type(message);
    if (teep_update->type != TEEP_TYPE_UPDATE) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    teep_update->contains |= TEEP_MESSAGE_CONTAINS_TYPE;

    if (!qcbor_get_next(message, QCBOR_TYPE_MAP, &item, &error)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    uint16_t map_count = item.val.uCount;
    for (size_t i = 0; i < map_count; i++) {
        if (!qcbor_get_next_nockeck(message, &item, &error)) {
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        switch (item.label.uint64) {
            case TEEP_OPTIONS_KEY_TOKEN:
                if (!qcbor_value_is_uint64(&item)) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                teep_update->token = item.val.uint64;
                teep_update->contains |= TEEP_MESSAGE_CONTAINS_TOKEN;
                break;
            case TEEP_OPTIONS_KEY_TC_LIST:
                result = set_teep_any_array(message, &item, &error, QCBOR_TYPE_ANY, &teep_update->tc_list);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_update->contains |= TEEP_MESSAGE_CONTAINS_TC_LIST;
                break;
            case TEEP_OPTIONS_KEY_MANIFEST_LIST:
                result = set_teep_any_array(message, &item, &error, QCBOR_TYPE_ANY, &teep_update->manifest_list);
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

int32_t set_teep_success(QCBORDecodeContext *message,
                         teep_success_t *teep_success) {
    QCBORItem  item;
    QCBORError error;
    int32_t result;

    teep_success->contains = TEEP_MESSAGE_CONTAINS_TYPE;
    teep_success->type = TEEP_TYPE_TEEP_SUCCESS;
    teep_success->type = 0U;
    INITIALIZE_TEEP_BUF(teep_success->msg);
    INITIALIZE_TEEP_BUF(teep_success->suit_reports);

    teep_success->type = get_teep_message_type(message);
    if (teep_success->type != TEEP_TYPE_TEEP_SUCCESS) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }

    if (!qcbor_get_next(message, QCBOR_TYPE_MAP, &item, &error)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    uint16_t map_count = item.val.uCount;
    for (size_t i = 0; i < map_count; i++) {
        if (!qcbor_get_next_nockeck(message, &item, &error)) {
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        switch (item.label.uint64) {
            case TEEP_OPTIONS_KEY_TOKEN:
                if (!qcbor_value_is_uint64(&item)) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                teep_success->token = item.val.uint64;
                teep_success->contains |= TEEP_MESSAGE_CONTAINS_TOKEN;
                break;
            case TEEP_OPTIONS_KEY_MSG:
                result = set_teep_byte_string(QCBOR_TYPE_TEXT_STRING, &item, &teep_success->msg);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_success->contains |= TEEP_MESSAGE_CONTAINS_MSG;
                break;
            case TEEP_OPTIONS_KEY_SUIT_REPORTS:
                result = set_teep_any_array(message, &item, &error, QCBOR_TYPE_ANY, &teep_success->suit_reports);
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

int32_t set_teep_error(QCBORDecodeContext *message,
                       teep_error_t *teep_error) {
    QCBORItem  item;
    QCBORError error;
    int32_t result;

    teep_error->contains = TEEP_MESSAGE_CONTAINS_TYPE;
    teep_error->type = TEEP_TYPE_TEEP_ERROR;
    teep_error->token = 0U;
    INITIALIZE_TEEP_BUF(teep_error->err_msg);
    INITIALIZE_TEEP_ARRAY(teep_error->supported_cipher_suites);
    INITIALIZE_TEEP_ARRAY(teep_error->versions);
    INITIALIZE_TEEP_BUF(teep_error->suit_reports);
    teep_error->err_code = TEEP_ERR_CODE_INVALID;

    teep_error->type = get_teep_message_type(message);
    if (teep_error->type != TEEP_TYPE_TEEP_ERROR) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }

    if (!qcbor_get_next(message, QCBOR_TYPE_MAP, &item, &error)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    uint16_t map_count = item.val.uCount;
    for (size_t i = 0; i < map_count; i++) {
        if (!qcbor_get_next_nockeck(message, &item, &error)) {
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        switch (item.label.uint64) {
            case TEEP_OPTIONS_KEY_TOKEN:
                if (!qcbor_value_is_uint64(&item)) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                teep_error->token = item.val.uint64;
                teep_error->contains |= TEEP_MESSAGE_CONTAINS_TOKEN;
                break;
            case TEEP_OPTIONS_KEY_ERR_MSG:
                result = set_teep_byte_string(QCBOR_TYPE_TEXT_STRING, &item, &teep_error->err_msg);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_error->contains |= TEEP_MESSAGE_CONTAINS_ERR_MSG;
                break;
            case TEEP_OPTIONS_KEY_SUPPORTED_CIPHER_SUITES:
                if (item.uDataType != QCBOR_TYPE_ARRAY) {
                    printf("\nset_teep_error : Error! uDataType = %d\n", item.uDataType);
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                if (item.val.uCount > TEEP_MAX_ARRAY_LENGTH) {
                    return TEEP_NO_MEMORY;
                }
                teep_error->supported_cipher_suites.len = item.val.uCount;
                for (size_t j = 0; j < teep_error->supported_cipher_suites.len; j++) {
                    if (!qcbor_get_next_uint64(message, &item, &error)) {
                        teep_error->supported_cipher_suites.len = j;
                        result = TEEP_INVALID_TYPE_OF_ARGUMENT;
                        break;
                    }
                    teep_error->supported_cipher_suites.items[j] = item.val.uint64;
                }
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_error->contains |= TEEP_MESSAGE_CONTAINS_SUPPORTED_CIPHER_SUITES;
                break;
            case TEEP_OPTIONS_KEY_VERSION:
                result = set_teep_any_array(message, &item, &error, QCBOR_TYPE_UINT32, &teep_error->versions);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_error->contains |= TEEP_MESSAGE_CONTAINS_VERSION;
                break;
            case TEEP_OPTIONS_KEY_SUIT_REPORTS:
                result = set_teep_byte_string(QCBOR_TYPE_BYTE_STRING, &item, &teep_error->suit_reports);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                teep_error->contains |= TEEP_MESSAGE_CONTAINS_SUIT_REPORTS;
                break;
            default:
                break;
        }
    }

    if (!qcbor_get_next_uint64(message, &item, &error)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    teep_error->err_code = item.val.uint64;
    teep_error->contains |= TEEP_MESSAGE_CONTAINS_ERR_CODE;
    return TEEP_SUCCESS;
}
