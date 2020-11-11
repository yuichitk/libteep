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
        printf("\ncbor_get_next : Error! uDataType = %d\n", item->uDataType);
        return false;
    }
    return true;
}

uint64_t get_teep_message_type(QCBORDecodeContext *message) {
    QCBORItem  item;
    QCBORError error;
    if (!qcbor_get_next(message, QCBOR_TYPE_ARRAY, &item, &error)) {
        return TEEP_TYPE_INVALID;
    }
    if (!qcbor_get_next(message, QCBOR_TYPE_INT64, &item, &error)) {
        return TEEP_TYPE_INVALID;
    }
    return item.val.uint64;
}

int32_t set_teep_type_token(QCBORDecodeContext *message,
                            QCBORItem *item,
                            QCBORError *error,
                            teep_type_t *type,
                            uint32_t *token) {
    if (!qcbor_get_next(message, QCBOR_TYPE_ARRAY, item, error)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    if (!qcbor_get_next(message, QCBOR_TYPE_INT64, item, error)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    *type = item->val.uint64;

    if (!qcbor_get_next(message, QCBOR_TYPE_INT64, item, error)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    *token = item->val.uint64;
    return TEEP_SUCCESS;
}

int32_t set_teep_uint_array(QCBORDecodeContext *message,
                            QCBORItem *item,
                            QCBORError *error,
                            teep_uint_array_t *uint_arr) {
    if (item->uDataType != QCBOR_TYPE_ARRAY) {
        printf("\nset_teep_uint_array : Error! uDataType = %d\n", item->uDataType);
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    uint_arr->len = item->val.uCount;
    for (size_t j = 0; j < uint_arr->len; j++) {
        if (!qcbor_get_next(message, QCBOR_TYPE_INT64, item, error)) {
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        uint_arr->items[j] = item->val.uint64;
    }
    return TEEP_SUCCESS;
}

int32_t set_teep_buf_array(QCBORDecodeContext *message,
                           QCBORItem *item,
                           QCBORError *error,
                           teep_buf_array_t *buf_arr) {
    if (item->uDataType != QCBOR_TYPE_ARRAY) {
        printf("\nset_teep_buf_array : Error! uDataType = %d\n", item->uDataType);
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    buf_arr->len = item->val.uCount;
    for (size_t j = 0; j < buf_arr->len; j++) {
        if (!qcbor_get_next(message, QCBOR_TYPE_BYTE_STRING, item, error)) {
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        buf_arr->items[j].ptr = item->val.string.ptr;
        buf_arr->items[j].len = item->val.string.len;
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

int32_t set_teep_query_request(QCBORDecodeContext *message,
                               teep_query_request_t *query_request) {
    QCBORItem  item;
    QCBORError error;
    int32_t result = set_teep_type_token(message, &item, &error, &query_request->type, &query_request->token);
    if (result != TEEP_SUCCESS) {
        return result;
    }

    if (!qcbor_get_next(message, QCBOR_TYPE_MAP, &item, &error)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    uint16_t map_count = item.val.uCount;
    query_request->supported_suites.len = 0;
    query_request->suite = TEEP_SUITE_INVALID;
    query_request->nonce.len = 0;
    query_request->version.len = 0;
    query_request->ocsp_data.len = 0;
    for (size_t i = 0; i < map_count; i++) {
        if (!qcbor_get_next_nockeck(message, &item, &error)) {
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        switch (item.label.uint64) {
            case TEEP_OPTIONS_KEY_CIPHER_SUITES:
                result = set_teep_uint_array(message, &item, &error, &query_request->supported_suites);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                break;
            case TEEP_OPTIONS_KEY_SELECTED_CIPHER_SUITE:
                if (item.uDataType != QCBOR_TYPE_INT64) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                query_request->suite = item.val.uint64;
                break;
            case TEEP_OPTIONS_KEY_NONCE:
                result = set_teep_byte_string(QCBOR_TYPE_BYTE_STRING, &item, &query_request->nonce);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                break;
            case TEEP_OPTIONS_KEY_VERSIONS:
                result = set_teep_uint_array(message, &item, &error, &query_request->version);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                break;
            case TEEP_OPTIONS_KEY_OSCP_DATA:
                result = set_teep_byte_string(QCBOR_TYPE_BYTE_STRING, &item, &query_request->ocsp_data);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                break;
            default:
                break;
        }
    }
    if (!qcbor_get_next(message, QCBOR_TYPE_INT64, &item, &error)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    query_request->data_item_requested = item.val.uint64;
    return TEEP_SUCCESS;
}

int32_t set_teep_query_response(QCBORDecodeContext *message,
                                teep_query_response_t *query_response) {
    QCBORItem  item;
    QCBORError error;
    int32_t result = set_teep_type_token(message, &item, &error, &query_response->type, &query_response->token);
    if (result != TEEP_SUCCESS) {
        return result;
    }

    if (!qcbor_get_next(message, QCBOR_TYPE_MAP, &item, &error)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    uint16_t map_count = item.val.uCount;
    query_response->suite = TEEP_SUITE_INVALID;
    query_response->version = TEEP_INVALID_VERSION;
    query_response->eat.len = 0;
    query_response->ta_list.len = 0;
    query_response->ext_info.len = 0;
    for (size_t i = 0; i < map_count; i++) {
        if (!qcbor_get_next_nockeck(message, &item, &error)) {
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        switch (item.label.uint64) {
            case TEEP_OPTIONS_KEY_SELECTED_CIPHER_SUITE:
                if (item.uDataType != QCBOR_TYPE_INT64) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                query_response->suite = item.val.uint64;
                break;
            case TEEP_OPTIONS_KEY_SELECTED_VERSION:
                if (item.uDataType != QCBOR_TYPE_INT64) {
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                query_response->version = item.val.uint64;
                break;
            case TEEP_OPTIONS_KEY_EAT:
                result = set_teep_byte_string(QCBOR_TYPE_BYTE_STRING, &item, &query_response->eat);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                break;
            case TEEP_OPTIONS_KEY_TA_LIST:
                result = set_teep_buf_array(message, &item, &error, &query_response->ta_list);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                break;
            case TEEP_OPTIONS_KEY_EXT_LIST:
                result = set_teep_uint_array(message, &item, &error, &query_response->ext_info);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                break;
            default:
                break;
        }
    }
    return TEEP_SUCCESS;
}

int32_t set_teep_install(QCBORDecodeContext *message,
                                     teep_install_t *app_install) {
    QCBORItem  item;
    QCBORError error;
    int32_t result = set_teep_type_token(message, &item, &error, &app_install->type, &app_install->token);
    if (result != TEEP_SUCCESS) {
        return result;
    }

    if (!qcbor_get_next(message, QCBOR_TYPE_MAP, &item, &error)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    uint16_t map_count = item.val.uCount;
    app_install->manifest_list.len = 0;
    for (size_t i = 0; i < map_count; i++) {
        if (!qcbor_get_next_nockeck(message, &item, &error)) {
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        switch (item.label.uint64) {
            case TEEP_OPTIONS_KEY_MANIFEST_LIST:
                result = set_teep_buf_array(message, &item, &error, &app_install->manifest_list);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                break;
            default:
                break;
        }
    }
    return TEEP_SUCCESS;
}

int32_t set_teep_delete(QCBORDecodeContext *message,
                                    teep_delete_t *app_delete) {
    QCBORItem  item;
    QCBORError error;
    int32_t result = set_teep_type_token(message, &item, &error, &app_delete->type, &app_delete->token);
    if (result != TEEP_SUCCESS) {
        return result;
    }

    if (!qcbor_get_next(message, QCBOR_TYPE_MAP, &item, &error)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    uint16_t map_count = item.val.uCount;
    app_delete->ta_list.len = 0;
    for (size_t i = 0; i < map_count; i++) {
        if (!qcbor_get_next_nockeck(message, &item, &error)) {
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        switch (item.label.uint64) {
            case TEEP_OPTIONS_KEY_TA_LIST:
                result = set_teep_buf_array(message, &item, &error, &app_delete->ta_list);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
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
    int32_t result = set_teep_type_token(message, &item, &error, &teep_success->type, &teep_success->token);
    if (result != TEEP_SUCCESS) {
        return result;
    }

    if (!qcbor_get_next(message, QCBOR_TYPE_MAP, &item, &error)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    uint16_t map_count = item.val.uCount;
    teep_success->msg.len = 0;
    for (size_t i = 0; i < map_count; i++) {
        if (!qcbor_get_next_nockeck(message, &item, &error)) {
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        switch (item.label.uint64) {
            case TEEP_OPTIONS_KEY_MSG:
                result = set_teep_byte_string(QCBOR_TYPE_TEXT_STRING, &item, &teep_success->msg);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
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
    int32_t result = set_teep_type_token(message, &item, &error, &teep_error->type, &teep_error->token);
    if (result != TEEP_SUCCESS) {
        return result;
    }

    if (!qcbor_get_next(message, QCBOR_TYPE_MAP, &item, &error)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    uint16_t map_count = item.val.uCount;
    teep_error->err_msg.len = 0;
    teep_error->suite.len = 0;
    teep_error->version.len = 0;
    for (size_t i = 0; i < map_count; i++) {
        if (!qcbor_get_next_nockeck(message, &item, &error)) {
            return TEEP_INVALID_TYPE_OF_ARGUMENT;
        }
        switch (item.label.uint64) {
            case TEEP_OPTIONS_KEY_ERR_MSG:
                result = set_teep_byte_string(QCBOR_TYPE_TEXT_STRING, &item, &teep_error->err_msg);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                break;
            case TEEP_OPTIONS_KEY_CIPHER_SUITES:
                if (item.uDataType != QCBOR_TYPE_ARRAY) {
                    printf("\nset_teep_error : Error! uDataType = %d\n", item.uDataType);
                    return TEEP_INVALID_TYPE_OF_ARGUMENT;
                }
                teep_error->suite.len = item.val.uCount;
                for (size_t j = 0; j < teep_error->suite.len; j++) {
                    if (!qcbor_get_next(message, QCBOR_TYPE_INT64, &item, &error)) {
                        return TEEP_INVALID_TYPE_OF_ARGUMENT;
                    }
                    teep_error->suite.items[j] = item.val.uint64;
                }
                break;
            case TEEP_OPTIONS_KEY_VERSIONS:
                result = set_teep_uint_array(message, &item, &error, &teep_error->version);
                if (result != TEEP_SUCCESS) {
                    return result;
                }
                break;
            default:
                break;
        }
    }

    if (!qcbor_get_next(message, QCBOR_TYPE_INT64, &item, &error)) {
        return TEEP_INVALID_TYPE_OF_ARGUMENT;
    }
    teep_error->err_code = item.val.uint64;
    return TEEP_SUCCESS;
}
